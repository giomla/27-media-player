#ifndef COMMANDS_H
#define COMMANDS_H

#endif // commands_H

#include <QAbstractButton>
#include <QSlider>
#include <QLabel>
#include <QHBoxLayout>
#include <QListWidget>
class commands : public QWidget{

public:
    commands(QWidget *parent = nullptr);
    QAbstractButton* m_playButton = nullptr;
    QAbstractButton* m_stopButton = nullptr;
    QAbstractButton* m_forwardButton = nullptr;
    QAbstractButton* m_backwardButton = nullptr;
    QAbstractButton* m_muteButton = nullptr;
    QAbstractButton* m_openButton = nullptr;
    QAbstractButton* m_seekForwardButton = nullptr;
    QAbstractButton* m_seekBackwardButton = nullptr;
    QRect screenGeometry;
    QSlider* m_Slider = nullptr;
    QSlider* m_volumeSlider = nullptr;
    QLabel* m_durationInfo = nullptr;
    QHBoxLayout* commandsLayout = nullptr;
    QHBoxLayout* sliderLayout = nullptr;
    QListWidget* m_playlist_entries = nullptr;

    void hideCommands();
    void showCommands();
    void enableAllCommands();
    void createCommandLayout();
    void createSliderLayout();
};

