/***********************************************************************
*
* discovery.c
*
* Perform PPPoE discovery
*
* Copyright (C) 1999 by Roaring Penguin Software Inc.
*
***********************************************************************/

#define _BSD_SOURCE
static char const RCSID[] =
"$Id: discovery.c,v 1.4 2005/03/22 10:22:32 paulus Exp $";

#include "pppoe.h"
#include "pppd/pppd.h"
#include "pppd/fsm.h"
#include "pppd/lcp.h"

#include <string.h>
#include <stdlib.h>
#include <errno.h>

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#ifdef HAVE_SYS_UIO_H
#include <sys/uio.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef USE_LINUX_PACKET
#include <sys/ioctl.h>
#include <fcntl.h>
#endif

#include <signal.h>

/**********************************************************************
*%FUNCTION: parseForHostUniq
*%ARGUMENTS:
* type -- tag type
* len -- tag length
* data -- tag data.
* extra -- user-supplied pointer.  This is assumed to be a pointer to int.
*%RETURNS:
* Nothing
*%DESCRIPTION:
* If a HostUnique tag is found which matches our PID, sets *extra to 1.
***********************************************************************/
void
parseForHostUniq(UINT16_t type, UINT16_t len, unsigned char *data,
		 void *extra)
{
    int *val = (int *) extra;
    if (type == TAG_HOST_UNIQ && len == sizeof(pid_t)) {
	pid_t tmp;
	memcpy(&tmp, data, len);
	if (tmp == getpid()) {
	    *val = 1;
	}
    }
}

/**********************************************************************
*%FUNCTION: packetIsForMe
*%ARGUMENTS:
* conn -- PPPoE connection info
* packet -- a received PPPoE packet
*%RETURNS:
* 1 if packet is for this PPPoE daemon; 0 otherwise.
*%DESCRIPTION:
* If we are using the Host-Unique tag, verifies that packet contains
* our unique identifier.
***********************************************************************/
int
packetIsForMe(PPPoEConnection *conn, PPPoEPacket *packet)
{
    int forMe = 0;

    /* If packet is not directed to our MAC address, forget it */
    if (memcmp(packet->ethHdr.h_dest, conn->myEth, ETH_ALEN)) return 0;

    /* If we're not using the Host-Unique tag, then accept the packet */
    if (!conn->useHostUniq) return 1;

    parsePacket(packet, parseForHostUniq, &forMe);
    return forMe;
}

/**********************************************************************
*%FUNCTION: parsePADOTags
*%ARGUMENTS:
* type -- tag type
* len -- tag length
* data -- tag data
* extra -- extra user data.  Should point to a PacketCriteria structure
*          which gets filled in according to selected AC name and service
*          name.
*%RETURNS:
* Nothing
*%DESCRIPTION:
* Picks interesting tags out of a PADO packet
***********************************************************************/
void
parsePADOTags(UINT16_t type, UINT16_t len, unsigned char *data,
	      void *extra)
{
    struct PacketCriteria *pc = (struct PacketCriteria *) extra;
    PPPoEConnection *conn = pc->conn;
    UINT16_t mru;
    int i;

    switch(type) {
    case TAG_AC_NAME:
	pc->seenACName = 1;
	if (conn->printACNames) {
	    printf("Access-Concentrator: %.*s\n", (int) len, data);
	}
	if (conn->acName && len == strlen(conn->acName) &&
	    !strncmp((char *) data, conn->acName, len)) {
	    pc->acNameOK = 1;
	}
	break;
    case TAG_SERVICE_NAME:
	pc->seenServiceName = 1;
	if (conn->printACNames && len > 0) {
	    printf("       Service-Name: %.*s\n", (int) len, data);
	}
	if (conn->serviceName && len == strlen(conn->serviceName) &&
	    !strncmp((char *) data, conn->serviceName, len)) {
	    pc->serviceNameOK = 1;
	}
	break;
    case TAG_AC_COOKIE:
	if (conn->printACNames) {
	    printf("Got a cookie:");
	    /* Print first 20 bytes of cookie */
	    for (i=0; i<len && i < 20; i++) {
		printf(" %02x", (unsigned) data[i]);
	    }
	    if (i < len) printf("...");
	    printf("\n");
	}
	conn->cookie.type = htons(type);
	conn->cookie.length = htons(len);
	memcpy(conn->cookie.payload, data, len);
	break;
    case TAG_RELAY_SESSION_ID:
	if (conn->printACNames) {
	    printf("Got a Relay-ID:");
	    /* Print first 20 bytes of relay ID */
	    for (i=0; i<len && i < 20; i++) {
		printf(" %02x", (unsigned) data[i]);
	    }
	    if (i < len) printf("...");
	    printf("\n");
	}
	conn->relayId.type = htons(type);
	conn->relayId.length = htons(len);
	memcpy(conn->relayId.payload, data, len);
	break;
	case TAG_PPP_MAX_PAYLOAD:
		if (len == sizeof(mru)) {
			memcpy(&mru, data, sizeof(mru));
			mru = ntohs(mru);
			if (mru >= ETH_PPPOE_MTU) {
				if (lcp_allowoptions[0].mru > mru)
					lcp_allowoptions[0].mru = mru;
				if (lcp_wantoptions[0].mru > mru)
					lcp_wantoptions[0].mru = mru;
				conn->seenMaxPayload = 1;
			}
		}
		break;
    case TAG_SERVICE_NAME_ERROR:
	if (conn->printACNames) {
	    printf("Got a Service-Name-Error tag: %.*s\n", (int) len, data);
	} else {
	    fatallog("PADO: Service-Name-Error: %.*s", (int) len, data);
	}
	break;
    case TAG_AC_SYSTEM_ERROR:
	if (conn->printACNames) {
	    printf("Got a System-Error tag: %.*s\n", (int) len, data);
	} else {
	    fatallog("PADO: System-Error: %.*s", (int) len, data);
	}
	break;
    case TAG_GENERIC_ERROR:
	if (conn->printACNames) {
	    printf("Got a Generic-Error tag: %.*s\n", (int) len, data);
	} else {
	    fatallog("PADO: Generic-Error: %.*s", (int) len, data);
	}
	break;
    }
}

/**********************************************************************
*%FUNCTION: parsePADSTags
*%ARGUMENTS:
* type -- tag type
* len -- tag length
* data -- tag data
* extra -- extra user data (pointer to PPPoEConnection structure)
*%RETURNS:
* Nothing
*%DESCRIPTION:
* Picks interesting tags out of a PADS packet
***********************************************************************/
void
parsePADSTags(UINT16_t type, UINT16_t len, unsigned char *data,
	      void *extra)
{
    PPPoEConnection *conn = (PPPoEConnection *) extra;
    UINT16_t mru;
    switch(type) {
    case TAG_SERVICE_NAME:
	dbglog("PADS: Service-Name: '%.*s'", (int) len, data);
	break;
	case TAG_PPP_MAX_PAYLOAD:
		if (len == sizeof(mru)) {
			memcpy(&mru, data, sizeof(mru));
			mru = ntohs(mru);
			if (mru >= ETH_PPPOE_MTU) {
				if (lcp_allowoptions[0].mru > mru)
					lcp_allowoptions[0].mru = mru;
				if (lcp_wantoptions[0].mru > mru)
					lcp_wantoptions[0].mru = mru;
				conn->seenMaxPayload = 1;
			}
		}
		break;
    case TAG_SERVICE_NAME_ERROR:
	fatallog("PADS: Service-Name-Error: %.*s", (int) len, data);
    case TAG_AC_SYSTEM_ERROR:
	fatallog("PADS: System-Error: %.*s", (int) len, data);
    case TAG_GENERIC_ERROR:
	fatallog("PADS: Generic-Error: %.*s", (int) len, data);
    case TAG_RELAY_SESSION_ID:
	conn->relayId.type = htons(type);
	conn->relayId.length = htons(len);
	memcpy(conn->relayId.payload, data, len);
	break;
    }
}

/***********************************************************************
*%FUNCTION: sendPADI
*%ARGUMENTS:
* conn -- PPPoEConnection structure
*%RETURNS:
* Nothing
*%DESCRIPTION:
* Sends a PADI packet
***********************************************************************/
void
sendPADI(PPPoEConnection *conn)
{
    PPPoEPacket packet;
    unsigned char *cursor = packet.payload;
    PPPoETag *svc = (PPPoETag *) (&packet.payload);
    UINT16_t namelen = 0;
    UINT16_t plen;

    if (conn->serviceName) {
	namelen = (UINT16_t) strlen(conn->serviceName);
    }
    plen = TAG_HDR_SIZE + namelen;
    CHECK_ROOM(cursor, packet.payload, plen);

    /* Set destination to Ethernet broadcast address */
    memset(packet.ethHdr.h_dest, 0xFF, ETH_ALEN);
    memcpy(packet.ethHdr.h_source, conn->myEth, ETH_ALEN);

    packet.ethHdr.h_proto = htons(Eth_PPPOE_Discovery);
    packet.ver = 1;
    packet.type = 1;
    packet.code = CODE_PADI;
    packet.session = 0;

    svc->type = TAG_SERVICE_NAME;
    svc->length = htons(namelen);
    CHECK_ROOM(cursor, packet.payload, namelen+TAG_HDR_SIZE);

    if (conn->serviceName) {
	memcpy(svc->payload, conn->serviceName, strlen(conn->serviceName));
    }
    cursor += namelen + TAG_HDR_SIZE;

    /* If we're using Host-Uniq, copy it over */
    if (conn->useHostUniq) {
	PPPoETag hostUniq;
	pid_t pid = getpid();
	hostUniq.type = htons(TAG_HOST_UNIQ);
	hostUniq.length = htons(sizeof(pid));
	memcpy(hostUniq.payload, &pid, sizeof(pid));
	CHECK_ROOM(cursor, packet.payload, sizeof(pid) + TAG_HDR_SIZE);
	memcpy(cursor, &hostUniq, sizeof(pid) + TAG_HDR_SIZE);
	cursor += sizeof(pid) + TAG_HDR_SIZE;
	plen += sizeof(pid) + TAG_HDR_SIZE;
    }

	/* Add our maximum MTU/MRU */
	if (MIN(lcp_allowoptions[0].mru, lcp_wantoptions[0].mru) > ETH_PPPOE_MTU) {
		PPPoETag maxPayload;
		UINT16_t mru = htons(MIN(lcp_allowoptions[0].mru, lcp_wantoptions[0].mru));
		maxPayload.type = htons(TAG_PPP_MAX_PAYLOAD);
		maxPayload.length = htons(sizeof(mru));
		memcpy(maxPayload.payload, &mru, sizeof(mru));
		CHECK_ROOM(cursor, packet.payload, sizeof(mru) + TAG_HDR_SIZE);
		memcpy(cursor, &maxPayload, sizeof(mru) + TAG_HDR_SIZE);
		cursor += sizeof(mru) + TAG_HDR_SIZE;
		plen += sizeof(mru) + TAG_HDR_SIZE;
	}

    packet.length = htons(plen);

    sendPacket(conn, conn->discoverySocket, &packet, (int) (plen + HDR_SIZE));
    if (conn->debugFile) {
	dumpPacket(conn->debugFile, &packet, "SENT");
	fprintf(conn->debugFile, "\n");
	fflush(conn->debugFile);
    }
}

/**********************************************************************
*%FUNCTION: waitForPADO
*%ARGUMENTS:
* conn -- PPPoEConnection structure
* timeout -- how long to wait (in seconds)
*%RETURNS:
* Nothing
*%DESCRIPTION:
* Waits for a PADO packet and copies useful information
***********************************************************************/
static int
waitForPADO(PPPoEConnection *conn, int timeout)
{
    fd_set readable;
    int r;
    struct timeval tv;
    struct timeval now;
    struct timeval to;
    PPPoEPacket packet;
    int len;

    struct PacketCriteria pc;
    pc.conn          = conn;
    pc.acNameOK      = (conn->acName)      ? 0 : 1;
    pc.serviceNameOK = (conn->serviceName) ? 0 : 1;
    pc.seenACName    = 0;
    pc.seenServiceName = 0;
    conn->seenMaxPayload = 0;
	
    gettimeofday(&now, NULL);
    to.tv_sec = now.tv_sec + timeout;
    to.tv_usec = now.tv_usec;
    do {
    	gettimeofday(&tv, NULL);
    	if (timercmp(&tv, &now, <) || /* protection against clock jump back */
    		timercmp(&tv, &to, >)) /* real timeout */
    		break; /* Real Timeout */
	if (BPF_BUFFER_IS_EMPTY) {
	    timersub(&to, &tv, &tv);
	
	    FD_ZERO(&readable);
	    FD_SET(conn->discoverySocket, &readable);

		r = select(conn->discoverySocket+1, &readable, NULL, NULL, &tv);
	    if (r < 0 && errno != EINTR)
	    {
	    	fatallog("waitForPADO: select: %m");
	    }
	    if (r < 0) {
        	return -1; /* Interrupted or Error... */
	    }
	    if (r == 0) continue; /* Timed out */
	}
	
	/* Get the packet */
	receivePacket(conn->discoverySocket, &packet, &len);

	/* Check length */
	if (ntohs(packet.length) + HDR_SIZE > len) {
	    errorlog("Bogus PPPoE length field (%u)",
		   (unsigned int) ntohs(packet.length));
	    continue;
	}

#ifdef USE_BPF
	/* If it's not a Discovery packet, loop again */
	if (etherType(&packet) != Eth_PPPOE_Discovery) continue;
#endif

	if (conn->debugFile) {
	    dumpPacket(conn->debugFile, &packet, "RCVD");
	    fprintf(conn->debugFile, "\n");
	    fflush(conn->debugFile);
	}
	/* If it's not for us, loop again */
	if (!packetIsForMe(conn, &packet)) continue;

	if (packet.code == CODE_PADO) {
	    if (BROADCAST(packet.ethHdr.h_source)) {
		errorlog("Ignoring PADO packet from broadcast MAC address");
		continue;
	    }
	    parsePacket(&packet, parsePADOTags, &pc);
	    if (!pc.seenACName) {
		errorlog("Ignoring PADO packet with no AC-Name tag");
		continue;
	    }
	    if (!pc.seenServiceName) {
		errorlog("Ignoring PADO packet with no Service-Name tag");
		continue;
	    }
	    conn->numPADOs++;
	    if (conn->printACNames) {
		printf("--------------------------------------------------\n");
	    }
	    if (pc.acNameOK && pc.serviceNameOK) {
		memcpy(conn->peerEth, packet.ethHdr.h_source, ETH_ALEN);
		if (conn->printACNames) {
		    printf("AC-Ethernet-Address: %02x:%02x:%02x:%02x:%02x:%02x\n",
			   (unsigned) conn->peerEth[0], 
			   (unsigned) conn->peerEth[1],
			   (unsigned) conn->peerEth[2],
			   (unsigned) conn->peerEth[3],
			   (unsigned) conn->peerEth[4],
			   (unsigned) conn->peerEth[5]);
		    continue;
		}
		conn->discoveryState = STATE_RECEIVED_PADO;
		break;
	    }
	}
    } while (conn->discoveryState != STATE_RECEIVED_PADO);
    return 0;
}

/***********************************************************************
*%FUNCTION: sendPADR
*%ARGUMENTS:
* conn -- PPPoE connection structur
*%RETURNS:
* Nothing
*%DESCRIPTION:
* Sends a PADR packet
***********************************************************************/
void
sendPADR(PPPoEConnection *conn)
{
    PPPoEPacket packet;
    PPPoETag *svc = (PPPoETag *) packet.payload;
    unsigned char *cursor = packet.payload;

    UINT16_t namelen = 0;
    UINT16_t plen;

    if (conn->serviceName) {
	namelen = (UINT16_t) strlen(conn->serviceName);
    }
    plen = TAG_HDR_SIZE + namelen;
    CHECK_ROOM(cursor, packet.payload, plen);

    memcpy(packet.ethHdr.h_dest, conn->peerEth, ETH_ALEN);
    memcpy(packet.ethHdr.h_source, conn->myEth, ETH_ALEN);

    packet.ethHdr.h_proto = htons(Eth_PPPOE_Discovery);
    packet.ver = 1;
    packet.type = 1;
    packet.code = CODE_PADR;
    packet.session = 0;

    svc->type = TAG_SERVICE_NAME;
    svc->length = htons(namelen);
    if (conn->serviceName) {
	memcpy(svc->payload, conn->serviceName, namelen);
    }
    cursor += namelen + TAG_HDR_SIZE;

    /* If we're using Host-Uniq, copy it over */
    if (conn->useHostUniq) {
	PPPoETag hostUniq;
	pid_t pid = getpid();
	hostUniq.type = htons(TAG_HOST_UNIQ);
	hostUniq.length = htons(sizeof(pid));
	memcpy(hostUniq.payload, &pid, sizeof(pid));
	CHECK_ROOM(cursor, packet.payload, sizeof(pid)+TAG_HDR_SIZE);
	memcpy(cursor, &hostUniq, sizeof(pid) + TAG_HDR_SIZE);
	cursor += sizeof(pid) + TAG_HDR_SIZE;
	plen += sizeof(pid) + TAG_HDR_SIZE;
    }

	/* Add our maximum MTU/MRU */
	if (MIN(lcp_allowoptions[0].mru, lcp_wantoptions[0].mru) > ETH_PPPOE_MTU) {
		PPPoETag maxPayload;
		UINT16_t mru = htons(MIN(lcp_allowoptions[0].mru, lcp_wantoptions[0].mru));
		maxPayload.type = htons(TAG_PPP_MAX_PAYLOAD);
		maxPayload.length = htons(sizeof(mru));
		memcpy(maxPayload.payload, &mru, sizeof(mru));
		CHECK_ROOM(cursor, packet.payload, sizeof(mru) + TAG_HDR_SIZE);
		memcpy(cursor, &maxPayload, sizeof(mru) + TAG_HDR_SIZE);
		cursor += sizeof(mru) + TAG_HDR_SIZE;
		plen += sizeof(mru) + TAG_HDR_SIZE;
	}

    /* Copy cookie and relay-ID if needed */
    if (conn->cookie.type) {
	CHECK_ROOM(cursor, packet.payload,
		   ntohs(conn->cookie.length) + TAG_HDR_SIZE);
	memcpy(cursor, &conn->cookie, ntohs(conn->cookie.length) + TAG_HDR_SIZE);
	cursor += ntohs(conn->cookie.length) + TAG_HDR_SIZE;
	plen += ntohs(conn->cookie.length) + TAG_HDR_SIZE;
    }

    if (conn->relayId.type) {
	CHECK_ROOM(cursor, packet.payload,
		   ntohs(conn->relayId.length) + TAG_HDR_SIZE);
	memcpy(cursor, &conn->relayId, ntohs(conn->relayId.length) + TAG_HDR_SIZE);
	cursor += ntohs(conn->relayId.length) + TAG_HDR_SIZE;
	plen += ntohs(conn->relayId.length) + TAG_HDR_SIZE;
    }

    packet.length = htons(plen);
    sendPacket(conn, conn->discoverySocket, &packet, (int) (plen + HDR_SIZE));
    if (conn->debugFile) {
	dumpPacket(conn->debugFile, &packet, "SENT");
	fprintf(conn->debugFile, "\n");
	fflush(conn->debugFile);
    }
}

/**********************************************************************
*%FUNCTION: waitForPADS
*%ARGUMENTS:
* conn -- PPPoE connection info
* timeout -- how long to wait (in seconds)
*%RETURNS:
* Nothing
*%DESCRIPTION:
* Waits for a PADS packet and copies useful information
***********************************************************************/
static int
waitForPADS(PPPoEConnection *conn, int timeout)
{
    fd_set readable;
    int r;
    struct timeval tv;
    struct timeval now;
    struct timeval to;
    PPPoEPacket packet;
    int len;

    gettimeofday(&now, NULL);
    to.tv_sec = now.tv_sec + timeout;
    to.tv_usec = now.tv_usec;
    do {
    	gettimeofday(&tv, NULL);
    	if (timercmp(&tv, &now, <) || /* protection against clock jump back */
    		timercmp(&tv, &to, >)) /* real timeout */
    		break; /* Real Timeout */
	if (BPF_BUFFER_IS_EMPTY) {
	    timersub(&to, &tv, &tv);
	    
	    FD_ZERO(&readable);
	    FD_SET(conn->discoverySocket, &readable);
	    
		r = select(conn->discoverySocket+1, &readable, NULL, NULL, &tv);
	    if (r < 0 && errno != EINTR)
	    {
        	fatallog("waitForPADS: select: %m");
	    }
	    if (r < 0) {
        	return -1; /* Interrupted or Error... */
	    }
	    if (r == 0) continue;
	}

	/* Get the packet */
	receivePacket(conn->discoverySocket, &packet, &len);

	/* Check length */
	if (ntohs(packet.length) + HDR_SIZE > len) {
	    errorlog("Bogus PPPoE length field (%u)",
		   (unsigned int) ntohs(packet.length));
	    continue;
	}

#ifdef USE_BPF
	/* If it's not a Discovery packet, loop again */
	if (etherType(&packet) != Eth_PPPOE_Discovery) continue;
#endif
	if (conn->debugFile) {
	    dumpPacket(conn->debugFile, &packet, "RCVD");
	    fprintf(conn->debugFile, "\n");
	    fflush(conn->debugFile);
	}

	/* If it's not from the AC, it's not for me */
	if (memcmp(packet.ethHdr.h_source, conn->peerEth, ETH_ALEN)) continue;

	/* If it's not for us, loop again */
	if (!packetIsForMe(conn, &packet)) continue;

	/* Is it PADS?  */
	if (packet.code == CODE_PADS) {
	    /* Parse for goodies */
	    parsePacket(&packet, parsePADSTags, conn);
	    conn->discoveryState = STATE_SESSION;
	    break;
	}
    } while (conn->discoveryState != STATE_SESSION);

    /* Don't bother with ntohs; we'll just end up converting it back... */
    conn->session = packet.session;

    infolog("PPP session is %d", ntohs(conn->session));

    /* RFC 2516 says session id MUST NOT be zero or 0xFFFF */
    if (ntohs(conn->session) == 0 || ntohs(conn->session) == 0xFFFF) {
	errorlog("Access concentrator used a session value of %x -- the AC is violating RFC 2516", (unsigned int) ntohs(conn->session));
    }
    return 0;
}

/**********************************************************************
*%FUNCTION: discovery
*%ARGUMENTS:
* conn -- PPPoE connection info structure
*%RETURNS:
* Nothing
*%DESCRIPTION:
* Performs the PPPoE discovery phase
***********************************************************************/
void
discovery(PPPoEConnection *conn)
{
    int padiAttempts = 0;
    int padrAttempts = 0;
    int timeout = PADI_TIMEOUT;

    /* Skip discovery and don't open discovery socket? */
    if (conn->skipDiscovery && conn->noDiscoverySocket) {
	conn->discoveryState = STATE_SESSION;
	return;
    }

    conn->discoverySocket =
	openInterface(conn->ifName, Eth_PPPOE_Discovery, conn->myEth);

    /* Skip discovery? */
    if (conn->skipDiscovery) {
	conn->discoveryState = STATE_SESSION;
	return;
    }

    do {
	padiAttempts++;
	if (padiAttempts > MAX_PADI_ATTEMPTS) {
	    warnlog("Timeout waiting for PADO packets");
	    close(conn->discoverySocket);
	    conn->discoverySocket = -1;
	    return;
	}
	sendPADI(conn);
	conn->discoveryState = STATE_SENT_PADI;
	if (waitForPADO(conn, timeout))
		padiAttempts = MAX_PADI_ATTEMPTS;

#if 0
	/* If we're just probing for access concentrators, don't do
	   exponential backoff.  This reduces the time for an unsuccessful
	   probe to 15 seconds. */
	if (!conn->printACNames) {
	    timeout *= 2;
	}
#endif
	if (conn->printACNames && conn->numPADOs) {
	    break;
	}
    } while (conn->discoveryState == STATE_SENT_PADI);

    /* If we're only printing access concentrator names, we're done */
    if (conn->printACNames) {
	exit(0);
    }

    timeout = PADI_TIMEOUT;
    do {
	padrAttempts++;
	if (padrAttempts > MAX_PADI_ATTEMPTS) {
	    warnlog("Timeout waiting for PADS packets");
	    close(conn->discoverySocket);
	    conn->discoverySocket = -1;
	    return;
	}
	sendPADR(conn);
	conn->discoveryState = STATE_SENT_PADR;
	if (waitForPADS(conn, timeout))
		padrAttempts = MAX_PADI_ATTEMPTS;
#if 0
	timeout *= 2;
#endif
    } while (conn->discoveryState == STATE_SENT_PADR);

	if (!conn->seenMaxPayload) {
		/* RFC 4638: MUST limit MTU/MRU to 1492 */
		if (lcp_allowoptions[0].mru > ETH_PPPOE_MTU)
			lcp_allowoptions[0].mru = ETH_PPPOE_MTU;
		if (lcp_wantoptions[0].mru > ETH_PPPOE_MTU)
			lcp_wantoptions[0].mru = ETH_PPPOE_MTU;
	}

    /* We're done. */
    conn->discoveryState = STATE_SESSION;
    return;
}

