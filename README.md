# ngrest

ngrest is a simple C++ REST framework. It has small footprint, extremely fast and very easy in use.

Some benchmark results https://docs.google.com/spreadsheets/d/1M8qtkO6dBa6Q2UPTeDApaohBF1_zTleF5XajcYipSG0/edit

ngrest is written on C++11 and uses CMake for build.

## Support

Feel free to ask ngrest related questions here: https://groups.google.com/forum/#!forum/ngrest

## Install

Currently only Linux and Windows supported, but there are plans to extend support for other popular operating systems.

Installation with screenshots for Linux: https://github.com/loentar/ngrest/wiki/Installation-guide-with-screenshots

Installation with screenshots for Windows: https://github.com/loentar/ngrest/wiki/Installation-guide-with-screenshots-Windows

To install ngrest, open terminal and copy-paste:

`wget -qO- http://bit.ly/ngrest | bash`


**Notes:**

1. If you don't have one of dependencies required, installer will ask you to enter your password to install it automatically. If you don't want this, press Ctrl+C and start this command: "sudo apt-get install git cmake g++". After apt-get finished, start the line above again.

2. By default script installs `ngrest` wrapper into best location available. If you have `~/bin` directory in your search path `ngrest` wrapper will be installed into it. Else it will try to install into `/usr/local/bin/` and you will be prompted for your password to install. To override this behavior and forcibly install `ngrest` wrapper into `~/bin` please create `~/bin` directory and re-login. After re-login it should be added into `$PATH` automatically. If this does not happen, please add into your `~/.bashrc` or `~/.profile` a line: `export PATH=$PATH:$USER/bin`. Also you can export `USERINST` environment variable to something non-empty, install ngrest and re-login.

## Create a new project

Create and start project with screenshots: https://github.com/loentar/ngrest/wiki/Creating-and-starting-a-project-guide-with-screenshots

To create a new project please open new terminal and enter:

`ngrest create <project_name>`

Where `<project_name>` is a name of your project and a service name.

Example 1. Create project 'calculator' and a service 'Calculator':

`ngrest create Calculator`

Optionally you can set up additional services and define it's namespaces.

Example 2. Create project 'calc' and two services - 'org.example.Calculator' and 'org.example.DivService':

`ngrest create calc org.example.Calculator org.example.DivService`

## Start the project

When a project is generated, an `echo` operation is added into each service. It's only provided as fast example and can be safely removed when you write your own operations.

You can start your project right after it's created:

```
cd calc
ngrest
```

ngrest wrapper will build your project and will start the server on default port.

After server is started you can try your service operations: open a link located below the `To test your services try ngrest service tester:` message in your browser, click "echo" link, enter something into "text" field and press "Submit".


## Implementing the service

Service's sources are located in `<servicename>/src/<ServiceName>.h/cpp` files. To implement your service you must edit those files (QtCreator is a very good tool for that: open CMakeLists.txt from project's dir in QtCreator).

> While you change source code, you can leave project started. ngrest will detect any changes in source code and will try to build and to apply changes. In case of successful build ngrest will restart the server.

> If you faced with crash ngrest will try to trace the error using gdb and display the place of crash and program stack. To restart the server just modify your source code.

Example. Add "add" operation into Calculator service:

1) insert this line into `Calculator.h` before end of class:

`int add(int a, int b);`

to make your service class appear like that:

```
class Calculator: public ngrest::Service
{
public:
    // ...
    std::string echo(const std::string& text);

    int add(int a, int b);
};
```

2) add implementation - append these lines into `Calculator.cpp`:

```
int Calculator::add(int a, int b)
{
  return a + b;
}
```

After that, click on the service name in service tester to see and test new `add` operation.

## Upgrade ngrest

To upgrade ngrest to the latest changeset from master branch type:

`ngrest upgrade`

If you want to downgrade to specific commit, add commit hash as last argument, for example:

`ngrest upgrade 3b78eee`

If there are any running projects it will be automatically rebuild and restarted.
