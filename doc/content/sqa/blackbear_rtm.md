!template load file=app_rtm.md.template app=BlackBear category=blackbear stp_filename=blackbear_rtm.md

!template! item key=system-scope
!include blackbear_srs.md start=system-scope-begin end=system-scope-finish
!template-end!

!template! item key=system-purpose
!include blackbear_srs.md start=system-purpose-begin end=system-purpose-finish
!template-end!
