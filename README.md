# jav8

## Introduction

The idea behind this project is to provide V8 as a JVM Scripting Engine (aka JSR-223).

### What's new?

  * Maven is being used for the builds using the [nar-maven-plugin](https://github.com/duns/maven-nar-plugin/)
  * Some smaller changes in style and fixes

### Original Repositories

This is my fork from [Flier Lu](https://github.com/flier)'s project. This is [a link to the original svn repository](http://code.google.com/p/jav8/). 
Another interesting fork [can be found here](https://github.com/gfodor/jav8) by [Greg Fodor](https://github.com/gfodor).

## Install/Build

1. Make sure you have maven 3.x and a C Compiler (gcc, mingw) installed.
2. Clone into a folder the v8 project:
```sh

    $ git clone git://github.com/v8/v8.git
```
3. Build V8:
```sh

    $ make native library=shared
```
4. Add to your settings.xml the property v8.path (where your v8 clone is located)
```xml
    ...
    <profiles>
      <profile>
        <id>default</id>
        <properties>
          <v8.path>/path/to/v8</v8.path>
        </properties>

        ...

      </profile>
    </profiles>
    ...
```
5. Run
```sh

    $ mvn clean install
```
6. Enjoy!

## Platforms/OS Supported

Currently, I have only tested this on Mac OS X but if it is not working out-of-the-box it should be no hard than to tweak a couple of the nar-maven-plugin options.

Happy hacking!
