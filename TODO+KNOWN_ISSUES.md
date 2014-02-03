## Known Issues:
* svdcoreginx_helper sucks, it's written badly and it doesn't reload Coreginx on first svdply deploy.
* LiveUsers igniter should be more intelligent. It now reloads ttys even if there were no changes in user autostart trigger.
* When user deletes ~/.autostart ttys should be altered (entry removed).
* Sometimes operation of service destroy doesn't work well (big recursive data dirs issue) and requires several attempts to get it done.
* Deployer asynchronous behavior requires bin/build on web-app side to be synchronous. Probably whole deployment should be fully synchronous to achieve reliable workload with possible issues during deployment (user errors).

## TODO list (in order of importance):
* Multiple domains for one service support. Difficulty approx.: 6/10.
* Auto scaling support (distributed architecture support). Difficulty approx.: 3/10.
* Implement software conflict check before run (error notification when f.e. Ruby is a requirement of service1 and Ruby19 is a requirement of service2). Difficulty approx.: 8/10.
* Add support for TheSS configuration (get rid of a couple of hardcodes, f.e.: Shell path). Difficulty approx.: 5/10.
* Internal/ External networks support (VPN/ Internet). Difficulty approx.: 7/10.