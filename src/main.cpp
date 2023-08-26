#include <QApplication>

#include "../headers/videoplayer.h"

auto main(int argc, char *argv[]) -> int
{
	QApplication app(argc, argv);


	videoplayer player;
	if (player.isAvaliable()) {
		player.show();
	}
	return app.exec();
}
