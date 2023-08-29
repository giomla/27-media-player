#ifndef COMMANDS_H
#define COMMANDS_H

#endif // commands_H

#include <QAbstractButton>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QSlider>
class commands : public QWidget {
public:
  commands(QWidget *parent = nullptr);
  QAbstractButton *m_playButton = nullptr;
  QAbstractButton *m_stopButton = nullptr;
  QAbstractButton *m_forwardButton = nullptr;
  QAbstractButton *m_backwardButton = nullptr;
  QAbstractButton *m_muteButton = nullptr;
  QAbstractButton *m_openButton = nullptr;
  QAbstractButton *m_seekForwardButton = nullptr;
  QAbstractButton *m_seekBackwardButton = nullptr;
  QAbstractButton *m_showPlaylistButton = nullptr;
  QRect screenGeometry;
  QSlider *m_Slider = nullptr;
  QSlider *m_volumeSlider = nullptr;
  QLabel *m_durationInfo = nullptr;
  QHBoxLayout *commandsLayout = nullptr;
  QHBoxLayout *sliderLayout = nullptr;

  /*! function that hides gui commands when video player is at full screen */
  void hideCommands();
  /*! fuction that shows back commands after exiting full screen */
  void showCommands();
  /*! fuction that enables all commands after video is loaded */
  void enableAllCommands();
  /*! fuction that creates command layout */
  void createCommandLayout();
  /*! function that creates slider layout */
  void createSliderLayout();
};
