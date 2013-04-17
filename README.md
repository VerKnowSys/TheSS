
## Authors:
* Daniel (dmilith) Dettlaff (dmilith [at] verknowsys.com). I'm also on #freebsd and #scala.pl @ freenode IRC.


## Contributors:
* Michał (tallica) Lipski.


## Dependencies:
* [Qt4 4.8.x](http://qt-project.org/downloads) (only QtCore part)


## Features
* Integrates with [Sofin](http://verknowsys.github.io/sofin)
* Support software igniters written in JSON (+comments support) to generate predefined (user or system wide) software configurations on the fly.
* Support for software hooks. (More in [Defaults.json](https://github.com/VerKnowSys/TheSS/blob/master/basesystem/universal/Default.json)). Hook example:

```json
    "install": {
        "commands": "sofin install mysoft; install my software plugin",
        "expectOutput": "Success happened, and this output is a confirmation",
        "expectOutputTimeout": 0
    },
```

* Using kernel file watchers (kqueue on OSX & BSD, epoll on Linux) to watch software config/ data dirs.
* Supports "touch .hookname" method of manual launching of hooks. (for example: touch ~/SoftwareData/MyApp/.install will invoke install hook of MyApp igniter definition.)
* Supports basic hook dependency model. (for example: touch ~/SoftwareData/MyApp/.start will invoke: install hook, configure hook, validate hook and start hook of MyApp igniter definition)
* Supports software hook expectations with error reporting through ~/SoftwareData/MyApp/.errors file.
* Supports software "baby sitting". By default watches for software pid ("alwaysOn" option in [Defaults.json](https://github.com/VerKnowSys/TheSS/blob/master/basesystem/universal/Default.json))
* Supports software TCP server checking. ("watchPort" option in [Defaults.json](https://github.com/VerKnowSys/TheSS/blob/master/basesystem/universal/Default.json))
* Supports free port checking for TCP services, automatically generates random port for new services, (but also supports "staticPort" option in [Defaults.json](https://github.com/VerKnowSys/TheSS/blob/master/basesystem/universal/Default.json)), to provide static port binding for software. (for example staticPort is 80 for Nginx). By default, service ports are stored in ~/SoftwareData/AppName/.ports and reused on next software start.
* Supports service autostarting. You need only to touch ~/SoftwareData/MyApp/.autostart file.
* Is designed to work with user privileges as a little user side daemon.
* Uses almost no memory: ~5 MiB RSS on 64bit system when working with several services.
* Supports super user mode, to support root services (ports: 1-1024). Just run svdss as root and that's it.
* Should be system agnostic. I didn't try windows, but it should work after few minor changes.
* Supports state files. For example if process is running the lock file is created: ~/SoftwareData/MyApp/.running
* Supports unified process configuration model. By default ~/SoftwareData/MyApp/service.pid and ~/SoftwareData/MyApp/service.log are created for each software.
* Tested in production environments.


## WTF? How to run it?

```sh
bin/build # to build svdss

bin/ignitersinstall # as root, on linux only - required to install common software igniters to /Users/Common (/home/Common on Linux)

./svdss # to just run user daemon
# do: mkdir ~/SoftwareData/Redis && touch ~/SoftwareData/Redis/.start
# wait a while, software install process should work in background (requires Sofin to be already installed!). This process has own log file here: ~/SoftwareData/Redis/.output. Software should pick random port, generate configuration for Redis service and just start it.
```


## Used 3rd party software and licenses info:
* JSON CPP implementation with JSON comments support by Baptiste Lepilleur (MIT licensed)
* CuteLogger MT logger implementation by Boris Moiseev (LGPL licensed)
* QT4 4.8.x implementation (LGPL licensed)
