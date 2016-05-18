# ngrest

ngrest is a simple C++ REST framework. It has small footprint, [extremely fast](https://docs.google.com/spreadsheets/d/1M8qtkO6dBa6Q2UPTeDApaohBF1_zTleF5XajcYipSG0/edit) and very easy in use.

ngrest is written on C++11 and uses CMake for build.

## Quick tour

### Hello world

No coding needed to make Hello world service working!

```bash
ngrest create HelloWorld
cd helloworld
ngrest
```

That's all! Now you can browse service operations and try HelloWorld service using ngrest service tester: [http://localhost:9098/ngrest/service/HelloWorld](http://localhost:9098/ngrest/service/HelloWorld).

Upon creation the test `echo` resource added to every service and by the address [http://localhost:9098/helloworld/echo?text=YOUR_TEXT](http://localhost:9098/helloworld/echo?text=YOUR_TEXT) this service will respond:

```JSON
{"result": "Hi, YOUR_TEXT"}
```

### Handling simple data types:

ngrest supports simple C++ data types:

```C++
class Calculator: public ngrest::Service {
public:
    int add(int a, int b) {
        return a + b;
    }

    int sub(int a, int b) {
        return a - b;
    }
};
```

To the request [http://localhost:9098/Calculator/add?a=2&b=3](http://localhost:9098/Calculator/add?a=2&b=3) this service will respond:

```JSON
{"result":5}
```

### Handling complex data types

ngrest supports [complex C++ data types](https://github.com/loentar/ngrest/wiki/Using-complex-data-types) such as structures, enums, typedefs and STL containers:

```C++
struct Pet {
    enum class Species {
        canine,
        feline
    };

    Species species;
    std::string name;
    std::string birthday;
};

class Pets: public ngrest::Service {
public:
    std::list<Pet> getPets() {
        return std::list<Pet>({{
            Pet::Species::canine,
            "spot",
            "Jan 1, 2010"
        },{
            Pet::Species::feline,
            "puff",
            "July 4, 2014"
        }});
    }
};
```

To the request [http://localhost:9098/Pets/getPets](http://localhost:9098/Pets/getPets) this service will respond:

```JSON
{"result":[{"species":"canine","name":"spot","birthday":"Jan 1, 2010"},{"species":"feline","name":"puff","birthday":"July 4, 2014"}]}
```

### Setting up HTTP method and location

To set HTTP method and location special comments are used:

```C++
// *location: /notes
class Notes: public ngrest::Service {
public:
    //! adds a new note
    // *method: POST
    // *location: /new
    std::string add(const std::string& text);

    //! gets all notes
    // *location: /all
    std::map<std::string, std::string> getAll();

    //! get a note by id
    // *location: /{id}
    std::string get(const std::string& id);

    //! deletes a note by id
    // *method: DELETE
    // *location: /{id}
    std::string remove(const std::string& id);
};
```

Please note, default method is `GET` and default location is equals to the name.

This will create a REST service on root path `/notes`. Also this will add four resources:


|**Operation**|**Method**|**URL**|**Request**|**Response**|
|-------------|----------|-------|-----------|------------|
| add         | POST     | http://localhost:9098/notes/new | `{"text":"Example text of the note"}` | `{"result":"d90638e1"}` |
| getAll      | GET      | http://localhost:9098/notes/all | | `{"result":[{"d90638e1":"Example text of the note"}]}` |
| get         | GET      | http://localhost:9098/notes/d90638e1 | | `{"result":"Example text of the note"}` |
| remove      | DELETE   | http://localhost:9098/notes/d90638e1 | | |


## Install

Currently only Linux and Windows supported, but there are plans to extend support for other popular operating systems.

Under Linux installation process is simple. To install ngrest, just open terminal and copy-paste:

```bash
wget -qO- http://bit.ly/ngrest | bash
```

[Installation guide with screenshots for Linux](https://github.com/loentar/ngrest/wiki/Installation-guide-with-screenshots)

[Installation guide with screenshots for Windows](https://github.com/loentar/ngrest/wiki/Installation-guide-with-screenshots-Windows)


**Notes:**

1. If you don't have one of dependencies required, installer will ask you to enter your password to install it automatically. If you don't want this, press Ctrl+C and start this command: "sudo apt-get install git cmake g++". After apt-get finished, start the line above again.

2. By default script installs `ngrest` wrapper into best location available. If you have `~/bin` directory in your search path `ngrest` wrapper will be installed into it. Else it will try to install into `/usr/local/bin/` and you will be prompted for your password to install. To override this behavior and forcibly install `ngrest` wrapper into `~/bin` please create `~/bin` directory and re-login. After re-login it should be added into `$PATH` automatically. If this does not happen, please add into your `~/.bashrc` or `~/.profile` a line: `export PATH=$PATH:$USER/bin`. Also you can export `USERINST` environment variable to something non-empty, install ngrest and re-login.

## Create a new project

[How to create and start project with screenshots](https://github.com/loentar/ngrest/wiki/Creating-and-starting-a-project-guide-with-screenshots)

To create a new project please open new terminal and enter:

```
ngrest create <project_name>
```

Or if you prefer to do not split the service to interface (.h) and implementation (.cpp) and develop the whole service within single `.hpp` file, add `-d hpp` option and ngrest will create hpp-style service(s) for you:

```
ngrest create -d hpp <project_name>
```


Where `<project_name>` is a name of your project and a service name.

Example 1. Create project 'calculator' and a service 'Calculator':

```
ngrest create Calculator
```

Optionally you can set up additional services and define it's namespaces.

Example 2. Create project 'calc' and two services - 'org.example.Calculator' and 'org.example.DivService':

```
ngrest create calc org.example.Calculator org.example.DivService
```

## Start the project

When a project is generated, an `echo` operation is added into each service. It's only provided as fast example and can be safely removed when you write your own operations.

You can start your project right after it's created:

```bash
cd calc
ngrest
```

ngrest wrapper will build your project and will start the server on default port.

After server is started you can try your service operations: open a link located below the `To test your services try ngrest service tester:` message in your browser, click "echo" link, enter something into "text" field and press "Submit".


## Implementing the service

Service's sources are located in `<servicename>/src/<ServiceName>.h/cpp/hpp` files. To implement your service you must edit those files (QtCreator is a very good tool for that: open CMakeLists.txt from project's dir in QtCreator).

> While you change source code, you can leave project started. ngrest will detect any changes in source code and will try to build and to apply changes. In case of successful build ngrest will restart the server.

> If you faced with crash ngrest will try to trace the error using gdb and display the place of crash and program stack. To restart the server just modify your source code.

Example. Add "add" operation into Calculator service:

1) if you use hpp-style service insert these lines into `Calculator.hpp` before end of class:
```C++
int add(int a, int b)
{
    return a + b;
}
```

to make your service class appear like this:

```C++
class Calculator: public ngrest::Service
{
public:
    // ...
    std::string echo(const std::string& text)
    {
        return "Hi, " + text;
    }

    int add(int a, int b)
    {
        return a + b;
    }
};
```

2) If you use h/cpp-style service, insert this line into `Calculator.h` before end of class:

```C++
int add(int a, int b);
```

to make your service class appear like this:

```C++
class Calculator: public ngrest::Service
{
public:
    // ...
    std::string echo(const std::string& text);

    int add(int a, int b);
};
```

and add implementation - append these lines into `Calculator.cpp`:

```C++
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

## Packages

Here is a list of packages to extend ngrest functinality:

|**Package**|**Repo URL**|**Description**|
|-----------|------------|---------------|
| loentar/ngrest-db | https://github.com/loentar/ngrest-db | Simple access to relational databases (Alpha). |

## TODO

 - Apache2 module
 - nginx module
 - nullable support
 - support complex types in services tester
 - support of sessions/cookies
 - WADL support?

## Support

Feel free to ask ngrest related questions here on the [Google groups](https://groups.google.com/forum/#!forum/ngrest).
