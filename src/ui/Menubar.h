#ifndef POTOPOTO_MENUBAR_H
#define POTOPOTO_MENUBAR_H

#include <string>
#include <sigslot/signal.hpp>

class Menubar {
public:
    Menubar() {}
    void Render();

    sigslot::signal<const std::string&>& OpenClicked() { return open_clicked; }
    sigslot::signal<>& SaveClicked() { return save_clicked; }
    sigslot::signal<>& CloseClicked() { return close_clicked; }

private:
    sigslot::signal<const std::string&> open_clicked;
    sigslot::signal<> save_clicked;
    sigslot::signal<> close_clicked;
};


#endif //POTOPOTO_MENUBAR_H
