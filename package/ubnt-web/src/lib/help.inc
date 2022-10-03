<?
Function localize_help $help_page (
      	global $active_language;
       	$h = "FULL_VERSION_LINK/help/"+$active_language+"/"+$help_page;
       	if (fileSize("."+$h) <= 0) {
       		$h = "FULL_VERSION_LINK/help/en_US/"+$help_page;
       	}
       	return $h;
);
>