#include <iostream>
#include <gtkmm/main.h>
#include <gtkmm/window.h>
#include <gtkmm/button.h>

class HelloWorldWindow : public Gtk::Window
{
public:
    HelloWorldWindow();

protected:
    Gtk::Button hello_world;
};

HelloWorldWindow::HelloWorldWindow()
 : hello_world("Hello World")
{
    // Set the title of the window.
    set_title("Hello World");

    // Add the member button to the window,
    add(hello_world);

    // Handle the 'click' event.
    hello_world.signal_clicked().connect([] () {
          std::cout << "Hello world" << std::endl;
    });
    // Display all the child widgets of the window.
    show_all_children();
}

int main(int argc, char *argv[]) 
{
    // Initialization
    Gtk::Main kit(argc, argv);

    // Create a hello world window object
    HelloWorldWindow example;

    // gtkmm main loop
    Gtk::Main::run(example);
    return 0;
}

