#include <linux/version.h>
#include <linux/types.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/config.h>
#include <linux/delay.h>
#include <linux/unistd.h>

#include "athrs_ioctl.h"
#include "athrs_config.h"
#include "athrs_mac.h"
/*Added by Bruce Liu. 2012-03-24*/
#define ATHR_LAN_PORT_VLAN          1
#define ATHR_WAN_PORT_VLAN          2
#define ENET_UNIT_LAN 1
#define ENET_UNIT_WAN 0

#define TRUE    1
#define FALSE   0

#define ATHR_PHY_MAX 5
#define ATHR_PHY0_ADDR   0x0
#define ATHR_PHY1_ADDR   0x1
#define ATHR_PHY2_ADDR   0x2
#define ATHR_PHY3_ADDR   0x3
#define ATHR_PHY4_ADDR   0x4
/*
 * Track per-PHY port information.
 */
typedef struct {
    BOOL   isEnetPort;       /* normal enet port */
    BOOL   isPhyAlive;       /* last known state of link */
    int    ethUnit;          /* MAC associated with this phy port */
    uint32_t phyBase;
    uint32_t phyAddr;          /* PHY registers associated with this phy port */
    uint32_t VLANTableSetting; /* Value to be written to VLAN table */
} athrPhyInfo_t;

/*
 * Per-PHY information, indexed by PHY unit number.
 */

static athrPhyInfo_t athPhyInfo[] = {

    {TRUE,   /* port 1 -- LAN port 1 */
     FALSE,
     ENET_UNIT_LAN,
     0,
     ATHR_PHY0_ADDR,
     ATHR_LAN_PORT_VLAN
    },

    {TRUE,   /* port 2 -- LAN port 2 */
     FALSE,
     ENET_UNIT_LAN,
     0,
     ATHR_PHY1_ADDR,
     ATHR_LAN_PORT_VLAN
    },

    {TRUE,   /* port 3 -- LAN port 3 */
     FALSE,
     ENET_UNIT_LAN,
     0,
     ATHR_PHY2_ADDR,
     ATHR_LAN_PORT_VLAN
    },

    {TRUE,   /* port 4 --  LAN port 4 */
     FALSE,
     ENET_UNIT_LAN,
     0,
     ATHR_PHY3_ADDR,
     ATHR_LAN_PORT_VLAN   /* Send to all ports */
    },

    {TRUE,  /* port 5 -- WAN Port 5 */
     FALSE,
     ENET_UNIT_WAN,
     0,
     ATHR_PHY4_ADDR,
     ATHR_LAN_PORT_VLAN    /* Send to all ports */
    },

    {FALSE,   /* port 0 -- cpu port 0 */
     TRUE,
     ENET_UNIT_LAN,
     0,
     0x00,
     ATHR_LAN_PORT_VLAN
    },
};

int  athr_mib_tx_stats(struct net_device *dev, struct ifreq *ifr, int phyBase)
{       
    
    athr_gmac_t *mac = (athr_gmac_t *) ATHR_MAC_PRIV(dev);
    athr_phy_ops_t *phy = mac->phy;

    struct eth_cfg_params *ethcfg;

    ethcfg = (struct eth_cfg_params *) ifr->ifr_data;
   
    ethcfg->txcntr.tx_broad	=  phy->read_mac_reg((athr_mibreg(ethcfg->portnum) | ATHR_PHY_TX_BROAD_REG));
    ethcfg->txcntr.tx_pause	=  phy->read_mac_reg((athr_mibreg(ethcfg->portnum) | ATHR_PHY_TX_PAUSE_REG));
    ethcfg->txcntr.tx_multi	=  phy->read_mac_reg((athr_mibreg(ethcfg->portnum) | ATHR_PHY_TX_MULTI_REG));
    ethcfg->txcntr.tx_underrun	=  phy->read_mac_reg((athr_mibreg(ethcfg->portnum) | ATHR_PHY_TX_MULTI_REG));
    ethcfg->txcntr.tx_64b	=  phy->read_mac_reg((athr_mibreg(ethcfg->portnum) | ATHR_PHY_TX_64B_REG));
    ethcfg->txcntr.tx_128b	=  phy->read_mac_reg((athr_mibreg(ethcfg->portnum) | ATHR_PHY_TX_128B_REG));
    ethcfg->txcntr.tx_256b	=  phy->read_mac_reg((athr_mibreg(ethcfg->portnum) | ATHR_PHY_TX_256B_REG));
    ethcfg->txcntr.tx_512b	=  phy->read_mac_reg((athr_mibreg(ethcfg->portnum) | ATHR_PHY_TX_512B_REG));
    ethcfg->txcntr.tx_1024b     =  phy->read_mac_reg((athr_mibreg(ethcfg->portnum) | ATHR_PHY_TX_1024B_REG));
    ethcfg->txcntr.tx_1518b     =  phy->read_mac_reg((athr_mibreg(ethcfg->portnum) | ATHR_PHY_TX_1518B_REG));
    ethcfg->txcntr.tx_maxb	=  phy->read_mac_reg((athr_mibreg(ethcfg->portnum) | ATHR_PHY_TX_MAXB_REG));
    ethcfg->txcntr.tx_oversiz	=  phy->read_mac_reg((athr_mibreg(ethcfg->portnum) | ATHR_PHY_TX_OVSIZE_REG));
    ethcfg->txcntr.tx_bytel   	=  phy->read_mac_reg((athr_mibreg(ethcfg->portnum) | ATHR_PHY_TX_TXBYTEL_REG));
    ethcfg->txcntr.tx_byteh  	=  phy->read_mac_reg((athr_mibreg(ethcfg->portnum) | ATHR_PHY_TX_TXBYTEU_REG));
    ethcfg->txcntr.tx_collision	=  phy->read_mac_reg((athr_mibreg(ethcfg->portnum) | ATHR_PHY_TX_COLL_REG));
    ethcfg->txcntr.tx_abortcol 	=  phy->read_mac_reg((athr_mibreg(ethcfg->portnum) | ATHR_PHY_TX_ABTCOLL_REG));
    ethcfg->txcntr.tx_multicol 	=  phy->read_mac_reg((athr_mibreg(ethcfg->portnum) | ATHR_PHY_TX_MLTCOL_REG));
    ethcfg->txcntr.tx_singalcol	=  phy->read_mac_reg((athr_mibreg(ethcfg->portnum) | ATHR_PHY_TX_SINGCOL_REG)); 
    ethcfg->txcntr.tx_execdefer	=  phy->read_mac_reg((athr_mibreg(ethcfg->portnum) | ATHR_PHY_TX_EXDF_REG));
    ethcfg->txcntr.tx_defer	=  phy->read_mac_reg((athr_mibreg(ethcfg->portnum) | ATHR_PHY_TX_DEF_REG));
    ethcfg->txcntr.tx_latecol	=  phy->read_mac_reg((athr_mibreg(ethcfg->portnum) | ATHR_PHY_TX_LATECOL_REG));
return 0;	
}
int athr_mib_rx_stats(struct net_device *dev, struct ifreq *ifr, int phyBase)
{
    athr_gmac_t *mac = (athr_gmac_t *) ATHR_MAC_PRIV(dev);
    athr_phy_ops_t *phy = mac->phy;

    struct eth_cfg_params *ethcfg;

    ethcfg = (struct eth_cfg_params *)ifr->ifr_data;

     ethcfg->rxcntr.rx_broad        = phy->read_mac_reg((athr_mibreg(ethcfg->portnum) | ATHR_PHY_RX_BROAD_REG));
     ethcfg->rxcntr.rx_pause        = phy->read_mac_reg((athr_mibreg(ethcfg->portnum) | ATHR_PHY_RX_PAUSE_REG));
     ethcfg->rxcntr.rx_multi        = phy->read_mac_reg((athr_mibreg(ethcfg->portnum) | ATHR_PHY_RX_MULTI_REG));
     ethcfg->rxcntr.rx_fcserr       = phy->read_mac_reg((athr_mibreg(ethcfg->portnum) | ATHR_PHY_RX_FCSERR_REG));
     ethcfg->rxcntr.rx_allignerr    = phy->read_mac_reg((athr_mibreg(ethcfg->portnum) | ATHR_PHY_RX_ALIGNERR_REG));
     ethcfg->rxcntr.rx_runt         = phy->read_mac_reg((athr_mibreg(ethcfg->portnum) | ATHR_PHY_RX_RUNT_REG));
     ethcfg->rxcntr.rx_frag         = phy->read_mac_reg((athr_mibreg(ethcfg->portnum) | ATHR_PHY_RX_FRAGMENT_REG));
     ethcfg->rxcntr.rx_64b          = phy->read_mac_reg((athr_mibreg(ethcfg->portnum) | ATHR_PHY_RX_RUNT_REG));
     ethcfg->rxcntr.rx_128b         = phy->read_mac_reg((athr_mibreg(ethcfg->portnum) | ATHR_PHY_RX_128B_REG));
     ethcfg->rxcntr.rx_256b         = phy->read_mac_reg((athr_mibreg(ethcfg->portnum) | ATHR_phy_RX_256B_REG));
     ethcfg->rxcntr.rx_512b         = phy->read_mac_reg((athr_mibreg(ethcfg->portnum) | ATHR_PHY_RX_512B_REG));
     ethcfg->rxcntr.rx_1024b        = phy->read_mac_reg((athr_mibreg(ethcfg->portnum) | ATHR_PHY_RX_1024B_REG));
     ethcfg->rxcntr.rx_1518b        = phy->read_mac_reg((athr_mibreg(ethcfg->portnum) | ATHR_PHY_RX_1518B_REG));
     ethcfg->rxcntr.rx_maxb         = phy->read_mac_reg((athr_mibreg(ethcfg->portnum) | ATHR_PHY_RX_MAXB_REG));
     ethcfg->rxcntr.rx_tool         = phy->read_mac_reg((athr_mibreg(ethcfg->portnum) | ATHR_PHY_RX_TOLO_REG));
     ethcfg->rxcntr.rx_goodbl       = phy->read_mac_reg((athr_mibreg(ethcfg->portnum) | ATHR_PHY_RX_GOODBL_REG));
     ethcfg->rxcntr.rx_goodbh       = phy->read_mac_reg((athr_mibreg(ethcfg->portnum) | ATHR_PHY_RX_GOODBU_REG));
     ethcfg->rxcntr.rx_overflow     = phy->read_mac_reg((athr_mibreg(ethcfg->portnum) | ATHR_PHY_RX_OVERFLW_REG));
     ethcfg->rxcntr.rx_badbl        = phy->read_mac_reg((athr_mibreg(ethcfg->portnum) | ATHR_PHY_RX_BADBL_REG));
     ethcfg->rxcntr.rx_badbu        = phy->read_mac_reg((athr_mibreg(ethcfg->portnum) | ATHR_PHY_RX_BADBU_REG));
           
          return 0;
}

int athr_forcephy_10M(struct net_device *dev, int phyBase, int phy_addr, int duplex)
{
    athr_gmac_t *mac = (athr_gmac_t *) ATHR_MAC_PRIV(dev);
    athr_phy_ops_t *phy = mac->phy;
    
	
    if (is_s26()) {
        /*power save off */
        phy->write_phy_reg(phyBase, phy_addr, ATHR_DEBUG_PORT_ADDRESS, 0x29);
        phy->write_phy_reg(phyBase, phy_addr, ATHR_DEBUG_PORT_DATA, 0x36c0);
        /*sleep off */
        phy->write_phy_reg(phyBase, phy_addr, ATHR_DEBUG_PORT_ADDRESS, 0xb);
        phy->write_phy_reg(phyBase, phy_addr, ATHR_DEBUG_PORT_DATA, 0x3c00);
    }
    
    phy->write_phy_reg(phyBase, phy_addr, ATHR_PHY_CONTROL,
                           (0x8000 | (duplex << 8)));

    return 0;
}

int athr_forcephy_100M(struct net_device *dev,int phyBase,  int phy_addr, int duplex)
{
    athr_gmac_t *mac = (athr_gmac_t *) ATHR_MAC_PRIV(dev);
    athr_phy_ops_t *phy = mac->phy;
    
    if (is_s26()) {	
        if (phy_addr % 2) {
            phy->write_phy_reg(phyBase, phy_addr, ATHR_PHY_FUNC_CONTROL, 0x820);
        } else {
        
            phy->write_phy_reg(phyBase, phy_addr, ATHR_PHY_FUNC_CONTROL, 0x800);
        }

        phy->write_phy_reg(phyBase, phy_addr, 0x1d, 0x29);
        phy->write_phy_reg(phyBase, phy_addr, 0x1e, 0x0);
        phy->write_phy_reg(phyBase, phy_addr, 0x10, 0xc60);
    }

    phy->write_phy_reg(phyBase, phy_addr, ATHR_PHY_CONTROL,
                       (0xa000 | (duplex << 8)));

    //mac->ops->check_link(mac,mac->mac_unit);
    return 0;
}
/*Added by Bruce Liu. 2012-03-24*/
struct p1020_config_param 
{
	int cmd;
	void * data;
};

struct eth_config 
{
    uint16_t port_num;
    uint8_t phy_or_global_reg;
    unsigned int index;
    unsigned int value;
};

typedef enum {
	ETH_REG_READ = 1,
	ETH_REG_WRITE,
	ETH_REG_MAX,
}eth_reg_operation_t;

#define mii_global_reg_read  1
#define mii_global_reg_write 2
#define mii_phy_reg_read     3
#define mii_phy_reg_write    4

/*Added by Bruce. 2012-03-25*/
unsigned int
ag934x_mii_reg_read(int unit, uint32_t phy_addr_global, unsigned int reg_addr)
{
    uint32_t reg_word_addr;
    uint32_t phy_addr, tmp_val, reg_val;
    uint16_t phy_val;
    uint16_t phy_reg;
	 
    /* change reg_addr to 16-bit word address, 32-bit aligned */
    reg_word_addr = (reg_addr & 0xfffffffc) >> 1;

    /* configure register high address */
    phy_addr = 0x18;
    phy_reg = 0x0;
    phy_val = (uint16_t) ((reg_word_addr >> 8) & 0x3ff);  /* bit16-8 of reg address */
    phy_reg_write(unit,phy_addr,phy_reg, phy_val);

    /* For some registers such as MIBs, since it is read/clear, we should */
    /* read the lower 16-bit register then the higher one */

    /* read register in lower address */
    phy_addr = 0x10 | ((reg_word_addr >> 5) & 0x7); /* bit7-5 of reg address */
    phy_reg = (uint16_t) (reg_word_addr & 0x1f);   /* bit4-0 of reg address */
    reg_val = (uint32_t) phy_reg_read(unit, phy_addr,phy_reg);

    /* read register in higher address */
    reg_word_addr++;
    phy_addr = 0x10 | ((reg_word_addr >> 5) & 0x7); /* bit7-5 of reg address */
    phy_reg = (uint16_t) (reg_word_addr & 0x1f);   /* bit4-0 of reg address */
    tmp_val = (uint32_t) phy_reg_read(unit, phy_addr,phy_reg);
    reg_val |= (tmp_val << 16);
	
    return reg_val;   
}

void
ag934x_mii_reg_write(int unit, uint32_t phy_addr_global, unsigned int reg_addr, unsigned int reg_val)
{
    uint32_t reg_word_addr;
    uint32_t phy_addr;
    uint16_t phy_val;
    uint16_t phy_reg;
   
    /* change reg_addr to 16-bit word address, 32-bit aligned */
    reg_word_addr = (reg_addr & 0xfffffffc) >> 1;

    /* configure register high address */
    phy_addr = 0x18;
    phy_reg = 0x0;
    phy_val = (uint16_t) ((reg_word_addr >> 8) & 0x3ff);  /* bit16-8 of reg address */
    phy_reg_write(unit,phy_addr,phy_reg, phy_val);

    /* For S17 registers such as ARL and VLAN, since they include BUSY bit */
    /* in higher address, we should write the lower 16-bit register then the */
    /* higher one */

    /* write register in lower address */
    phy_addr = 0x10 | ((reg_word_addr >> 5) & 0x7); /* bit7-5 of reg address */
    phy_reg = (uint16_t) (reg_word_addr & 0x1f);   /* bit4-0 of reg address */
    phy_val = (uint16_t) (reg_val & 0xffff);
    phy_reg_write(unit,phy_addr,phy_reg, phy_val);
    
    /* read register in higher address */
    reg_word_addr++;
    phy_addr = 0x10 | ((reg_word_addr >> 5) & 0x7); /* bit7-5 of reg address */
    phy_reg = (uint16_t) (reg_word_addr & 0x1f);   /* bit4-0 of reg address */
    phy_val = (uint16_t) ((reg_val >> 16) & 0xffff);
    phy_reg_write(unit,phy_addr,phy_reg, phy_val);
}
/*Ended by Bruce. 2012-03-25*/

int athr_phy_mii(struct net_device *dev, struct ifreq *ifr)
{
	struct p1020_config_param  *p_param = (struct p1020_config_param *)ifr->ifr_data;
    struct eth_config e_cfg;
    uint32_t      phyBase;
    uint32_t      phyAddr;
    int           port_num;

    //athr_gmac_t *mac = (athr_gmac_t *) ATHR_MAC_PRIV(dev);

    if(copy_from_user(&e_cfg, p_param->data, sizeof(e_cfg)))
        return -EFAULT;

    port_num = e_cfg.port_num;
	//phyBase = mac->mac_unit;
    //phyAddr = mac->mac_unit;
    //Added by Bruce. 2012-03-25
	phyBase = athPhyInfo[port_num].phyBase;
	phyAddr = athPhyInfo[port_num].phyAddr;
    
	switch (p_param->cmd) {
        case ETH_REG_READ:
            if(e_cfg.phy_or_global_reg == mii_global_reg_read) {
                e_cfg.value = ag934x_mii_reg_read(phyBase, phyAddr,e_cfg.index);
            }else if(e_cfg.phy_or_global_reg == mii_phy_reg_read){
			    e_cfg.value = phy_reg_read(phyBase, phyAddr,e_cfg.index);
            }else
                e_cfg.value = -1;
            
			if(copy_to_user(p_param->data, &e_cfg, sizeof(e_cfg)))
            	return -EFAULT;
            
            break;
        case ETH_REG_WRITE:
			/*if(copy_from_user(&e_cfg, p_param->data, sizeof(e_cfg)))
            	return -EFAULT;*/
            if(e_cfg.phy_or_global_reg == mii_global_reg_write) {
                ag934x_mii_reg_write(phyBase,phyAddr,e_cfg.index, e_cfg.value);
            }else if(e_cfg.phy_or_global_reg == mii_phy_reg_write){
			    phy_reg_write(phyBase,phyAddr,e_cfg.index, e_cfg.value);
            }
            
            break;
		default:
            break;
	}

	return 0;
}
/*End by Bruce*/
int athr_phy_ctrl(struct net_device *dev, struct ifreq *ifr, int cmd)
{
    int ret = 0;
    int i;
    int phyBase = 0;
    int flow_link_en = 0; 
    int phy_addr=0, duplex;
    int infmask = 0,portmask = 0;
    athr_gmac_t *mac = (athr_gmac_t *) ATHR_MAC_PRIV(dev);
    athr_phy_ops_t *phy = mac->phy;
    struct eth_cfg_params *ethcfg;
    ethcfg = (struct eth_cfg_params *) ifr->ifr_data;
	
    assert(ethcfg);
    assert(phy);

    phy_addr = ethcfg->portnum;
    
    infmask  = (1 << (6 + mac->mac_unit));

    if (phy_addr == 0x3f) 
        portmask = phy_addr;
    else 
        portmask = (1 << phy_addr);

    /* Sanity checks */

    if (!((phy->port_map & 0x3f) & portmask) 
           && !((phy->port_map & 0xc) & infmask))
    {
	printk("\nport_map:%x mask:%x\n",phy->port_map,portmask);
        return -EINVAL;
    }
    
    if (!(phy->read_mac_reg || phy->write_mac_reg))
        return -EINVAL;

    if (!(phy->read_phy_reg || phy->write_phy_reg))
        return -EINVAL;
    if (cmd != ATHR_PHY_CTRL_IOC)
        return -EINVAL;

    if (is_ar7240()) {
        phyBase = 0;
    }
    else if(is_ar7241() || is_ar7242() || is_ar934x()) {
        if (is_s17() || is_s16())
            phyBase = 0;
        else
            phyBase = 1;
    }

    cmd=ethcfg->cmd;

    switch (cmd) {

        case ATHR_PHY_RD:
            if (ethcfg->portnum != 0x3f) {
                ethcfg->val = phy->read_phy_reg(phyBase,
                                 ethcfg->portnum, ethcfg->phy_reg);
            } else {
                ethcfg->val = phy->read_mac_reg(ethcfg->phy_reg);
            }
            break;
        case ATHR_PHY_WR:
            if (ethcfg->portnum != 0x3f) {

                phy->write_phy_reg(phyBase,ethcfg->portnum,
                        ethcfg->phy_reg, ethcfg->val);
            } else {
                phy->write_mac_reg(ethcfg->phy_reg, ethcfg->val);
            }
            break;
        case ATHR_PHY_FORCE:
            duplex = ethcfg->duplex;
            
            if (ethcfg->phy_reg < ATHR_PHY_MAX) {
                if (ethcfg->val == 10) {

                    printk("Forcing 10Mbps %s on port:%d \n",
                           dup_str[ethcfg->duplex], (ethcfg->portnum));
                    athr_forcephy_10M(dev, phyBase, phy_addr, duplex);

                } else if (ethcfg->val == 100) {

                    printk("Forcing 100Mbps %s on port:%d \n",
                           dup_str[ethcfg->duplex], (ethcfg->portnum));
                    athr_forcephy_100M(dev, phyBase, phy_addr, duplex);

                } else if (ethcfg->val == 0) {
                    printk("Enabling Auto Neg on port:%d \n", (ethcfg->phy_reg));
                    phy->write_phy_reg(phyBase, ethcfg->portnum,
                                       ATHR_PHY_CONTROL, 0x9000);

                } else {
                    return -EINVAL;
		}
		printk("Phy status:%x\n", 
 			phy->read_phy_reg(phyBase,ethcfg->portnum,ATHR_PHY_CONTROL));

            } else
                return -EINVAL;        
            break;
        case ATHR_PHY_MIB :
            if (ethcfg->val == 1){
                 phy->write_mac_reg(ATHR_PHY_MIB_CTRL_REG,
                     (phy->read_mac_reg(ATHR_PHY_MIB_CTRL_REG) | ATHR_PHY_MIB_ENABLE));
                 printk("MIB counter enabled\n");
         
            }
            else if(ethcfg->val == 0){
                 phy->write_mac_reg(ATHR_PHY_MIB_CTRL_REG,
                      (phy->read_mac_reg(ATHR_PHY_MIB_CTRL_REG) & ~ATHR_PHY_MIB_ENABLE));
                 printk("MIB counter disabled\n");
            }
            else if(ethcfg->val == 0xff){
                 ret = phy->read_mac_reg(ATHR_PHY_MIB_CTRL_REG);
		 if(ret & ATHR_PHY_MIB_ENABLE)
                     printk("MIB module cntr set\n");
                 else
                     printk("MIB module cntr is not set\n"); 
	    }            
            else
               return -EINVAL;

            break;
        case ATHR_PORT_STATS:
        {
	     int portnum;
	     portnum = ethcfg->portnum +1;
                 
	     ethcfg->phy_st.link     = phy->is_alive(ethcfg->portnum);
             ethcfg->phy_st.speed    = phy->speed(mac->mac_unit, ethcfg->portnum);
             ethcfg->phy_st.duplex   = phy->is_fdx(mac->mac_unit, ethcfg->portnum);
             ethcfg->phy_st.txflctrl = ((phy->read_mac_reg(athr_flctrl_off(portnum )) >> 4 ) & 1);
             ethcfg->phy_st.rxflctrl = ((phy->read_mac_reg(athr_flctrl_off(portnum )) >> 5 ) & 1);

		
         }
            break;
        case ATHR_PHY_STATS:
            athr_mib_tx_stats(dev, ifr, phyBase); 
            athr_mib_rx_stats(dev, ifr, phyBase); 
            break;
        case ATHR_FLOW_LINK_EN:
            if (ethcfg->val == 1) {
                                
                phy->write_mac_reg(athr_flctrl_off(ethcfg->portnum),
                     (phy->read_mac_reg(athr_flctrl_off(ethcfg->portnum)) | (1 << 12)));

                printk("Flow link is enabled\n");
            } else if (ethcfg->val == 0) {
                phy->write_mac_reg(athr_flctrl_off(ethcfg->portnum),
                     (phy->read_mac_reg(athr_flctrl_off(ethcfg->portnum)) & ~(1 << 12)));

                printk("Flow link is disabled\n");
            }
            else {
                return -EINVAL;
            }
            break;
        case ATHR_PHY_RXFCTL:

            flow_link_en = (phy->read_mac_reg(athr_flctrl_off(ethcfg->portnum)) & (1 << 12));
                if (flow_link_en) {
                     phy->write_mac_reg(athr_flctrl_off(ethcfg->portnum), 
                               (phy->read_mac_reg(athr_flctrl_off(ethcfg->portnum)) & ~(1<<12)));

                     printk("Flow link disabled.\n");
                                     }
            if (ethcfg->val == 1) {
                
                phy->write_mac_reg(athr_flctrl_off(ethcfg->portnum),
                     (phy->read_mac_reg(athr_flctrl_off(ethcfg->portnum)) | ATHR_PHY_RXFCTL_EN));
                 

                printk ("RX Flow ctrl is enabled\n");

            } else if (ethcfg->val == 0) {

                phy->write_mac_reg(athr_flctrl_off(ethcfg->portnum),
                     (phy->read_mac_reg(athr_flctrl_off(ethcfg->portnum)) & ~ATHR_PHY_RXFCTL_EN));


                printk ("RX Flow ctrl is disabled\n");
            } else {
                return -EINVAL;
            }
            break;
        case ATHR_PHY_TXFCTL:

             flow_link_en = (phy->read_mac_reg(athr_flctrl_off(ethcfg->portnum)) & (1 << 12));
                if (flow_link_en) {

                    phy->write_mac_reg(athr_flctrl_off(ethcfg->portnum), 
                         (phy->read_mac_reg(athr_flctrl_off(ethcfg->portnum)) & ~(1<<12)));

                    printk("Flow link disabled.\n");
                }
            if (ethcfg->val == 1) {
                
                phy->write_mac_reg(athr_flctrl_off(ethcfg->portnum),
                     (phy->read_mac_reg(athr_flctrl_off(ethcfg->portnum)) | ATHR_PHY_TXFCTL_EN));


                printk ("TX Flow ctrl is enabled\n");

            } else if (ethcfg->val == 0) {
                phy->write_mac_reg(athr_flctrl_off(ethcfg->portnum),
                     (phy->read_mac_reg(athr_flctrl_off(ethcfg->portnum)) & ~ATHR_PHY_TXFCTL_EN));


                printk ("TX Flow ctrl is disabled\n");
            } else {
                return -EINVAL;
            }
            break;
        case ATHR_PHY_FLOW_CTRL:
            if (ethcfg->val == 1) { 

                printk("TX & RX Flow ctrl enabled in the switch side....\n");

                for (i = 1; i < MAX_PHY; i++ ) {
                    phy->write_mac_reg(athr_flctrl_off(i), 0x2b0);
                }

            } else if (ethcfg ->val == 0) {

                printk("TX & RX Flow ctrl disabled in the switch side....\n");

                for (i = 1; i < MAX_PHY; i++ ) {
                    phy->write_mac_reg(athr_flctrl_off(i), 0x280);
                }

                       
            } else {
                return -EINVAL;
            }
            break;
        default:
            break;
    }
    if (copy_to_user(ifr->ifr_data, ethcfg, sizeof(struct eth_cfg_params)))
        return -EFAULT;

    return 0;
}
