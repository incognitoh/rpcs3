#pragma once

#include "Utilities/File.h"
#include "Utilities/Log.h"
#include "3rdparty/IRCClient/src/IRCClient.h"

#include "custom_dock_widget.h"
#include "gui_settings.h"
#include "find_dialog.h"

#include <memory>

#include <QTabWidget>
#include <QTextEdit>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QActionGroup>
#include <QTimer>
#include <QKeyEvent>

class log_frame : public custom_dock_widget
{
	Q_OBJECT

public:
	explicit log_frame(std::shared_ptr<gui_settings> guiSettings, QWidget *parent = nullptr);

	/** Loads from settings. Public so that main_window can call this easily. */
	void LoadSettings();

	/** Repaint log colors after new stylesheet was applied */
	void RepaintTextColors();

	void MessageHandler(Irc::IRCMessage msg, Irc::IRCClient* c);

Q_SIGNALS:
	void LogFrameClosed();
protected:
	/** Override inherited method from Qt to allow signalling when close happened.*/
	void closeEvent(QCloseEvent* event) override;
	bool eventFilter(QObject* object, QEvent* event) override;
private Q_SLOTS:
	void UpdateUI();
private:
	void SetLogLevel(logs::level lev);
	void SetTTYLogging(bool val);
	void InitIRC();

	void CreateAndConnectActions();

	QTabWidget* m_tabWidget;

	std::unique_ptr<find_dialog> m_find_dialog;

	QList<QColor> m_color;
	QColor m_color_stack;
	QTextEdit* m_log;
	QTextEdit* m_tty;
	QString* m_nick;
	QString m_old_text;
	ullong m_log_counter;
	bool m_stack_log;

	QGroupBox* m_irc;
	QTextEdit* m_chat;
	QLineEdit* m_chat_text;
	QPushButton* m_send;

	fs::file m_tty_file;

	QAction* m_clearAct;
	QAction* m_clearTTYAct;

	QActionGroup* m_logLevels;
	QAction* m_nothingAct;
	QAction* m_fatalAct;
	QAction* m_errorAct;
	QAction* m_todoAct;
	QAction* m_successAct;
	QAction* m_warningAct;
	QAction* m_noticeAct;
	QAction* m_traceAct;

	QAction* m_stackAct;

	QAction* m_TTYAct;

	std::shared_ptr<gui_settings> xgui_settings;

	Irc::IRCClient client;
};
