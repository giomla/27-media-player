#include "../headers/videoplayer.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    //Ovde mozemo da dodamo ime aplikacije kad se dogovirmo.


    videoplayer player;
    if(player.isAvaliable()){
        //Load url-a kada se implementira.
        player.show();
    }
    return app.exec();
}
