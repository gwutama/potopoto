#ifndef POTOPOTO_MENUBAR_H
#define POTOPOTO_MENUBAR_H

#include <string>
#include <sigslot/signal.hpp>

class Menubar {
public:
    Menubar() {}
    void Render();

    sigslot::signal<const std::string&>& OpenSignal() { return open_signal; }
    sigslot::signal<>& SaveSignal() { return save_signal; }
    sigslot::signal<>& CloseSignal() { return close_signal; }

private:
    sigslot::signal<const std::string&> open_signal;
    sigslot::signal<> save_signal;
    sigslot::signal<> close_signal;
};


#endif //POTOPOTO_MENUBAR_H
