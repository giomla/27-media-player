#include <QApplication>

#include "../headers/videoplayer.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);


	videoplayer player;
	if (player.isAvaliable()) {
		player.show();
	}
	return app.exec();
}
