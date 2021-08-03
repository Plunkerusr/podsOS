#include "WindowServer.hpp"

#include <libc/malloc.hpp>
#include <libc/syscalls.hpp>

#include <libsys/Log.hpp>
#include <libsys/syscalls.hpp>

#include <libgraphics/Application.hpp>
#include <libgraphics/Bitmap.hpp>
#include <libgraphics/Color.hpp>

#include <wisterialib/posix/defines.hpp>

#include <libipc/ClientConnection.hpp>
#include <libipc/ServerConnection.hpp>

void run_demo()
{
    // if (!fork()) {
    //     auto app = Application();
    //     size_t cnt = 0;

    //     while (true) {
    //         for (int red = 0; red < 0xff; red += 10) {
    //             for (int green = 0; green < 0xff; green += 20) {
    //                 for (int blue = 0; blue < 0xff; blue += 30) {

    //                     for (size_t y = 0; y < app.window().height(); y++) {
    //                         for (size_t x = 0; x < app.window().width(); x++) {
    //                             app.window().buffer()[y][x] = Graphics::Color(red, green, blue);
    //                             if (cnt == 0) {
    //                                 app.invalidate_area(0, 0, app.window().width(), app.window().height());
    //                             }
    //                             cnt++;
    //                             if (cnt % 1000000 == 0) {
    //                                 app.invalidate_area(0, 0, app.window().width() / 2, app.window().height() / 2);
    //                             }
    //                         }
    //                     }

    //                     sched_yield();
    //                 }
    //             }
    //         }
    //     }
    // }

    if (!fork()) {
        Log << "INIT CLIENT" << endl;
        auto client = IPC::ClientConnection("/ext2/ws.socket");
        Log << "INIT DONE" << endl;
        client.send_data((void*)"HELLO", 6);
        client.send_data((void*)"HELLO", 6);
        client.send_data((void*)"HELLO", 6);
        client.send_data((void*)"HELLO", 6);
        exit(0);

    } else {
        auto server = IPC::ServerConnection("/ext2/ws.socket");
        while (true) {
            server.pump();
            auto messages = server.take_over_messages();
            for (auto& message : messages) {
                Log << "message pid from" << message.pid_from << endl;
                Log << "message: " << String(message.message.data()) << endl;
            }
        }
    }
}

int main()
{
    run_demo();
    run_demo();

    auto wm = WindowServer();
    if (wm.initialize()) {
        wm.run();
    }

    return 0;
}