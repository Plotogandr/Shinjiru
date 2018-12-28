#include "application.h"
#include "ui_application.h"

#include <QDir>
#include <QDirIterator>

#include "../../../src/paths.h"

#include "../../../src/settings.h"

namespace Views {
Application::Application(QWidget *parent) : CommittableWidget(parent), ui(new Ui::Application) {
  ui->setupUi(this);

  QDir theme_dir(Paths::configDir("themes"));

  QDirIterator it(theme_dir);

  while (it.hasNext()) {
    it.next();
    auto file = it.fileName();

    if (file != "." && file != "..") {
      ui->theme->addItem(file.left(file.lastIndexOf(".")));
    }
  }

  Settings s;

  auto language = s.get(Setting::Language).toString();
  auto startOnBoot = s.get(Setting::StartOnBoot).toBool();
  auto checkForUpdates = s.get(Setting::CheckForUpdates).toBool();
  auto startMinimized = s.get(Setting::StartMinimized).toBool();
  auto minimizeToTray = s.get(Setting::MinimizeToTray).toBool();
  auto closeToTray = s.get(Setting::CloseToTray).toBool();
  auto theme = s.get(Setting::Theme).toString();

  ui->language->setCurrentText(language);
  ui->startOnBoot->setChecked(startOnBoot);
  ui->checkForUpdates->setChecked(checkForUpdates);
  ui->startMinimized->setChecked(startMinimized);
  ui->minimizeToTray->setChecked(minimizeToTray);
  ui->closeToTray->setChecked(closeToTray);
  ui->theme->setCurrentText(theme);

  connect(ui->language, &QComboBox::currentTextChanged, this, [this](const QString &language) {
    this->changed_settings[Setting::Language] = language;
    this->restart_required = true;
  });

  connect(ui->theme, &QComboBox::currentTextChanged, this, [this](const QString &theme) {
    this->changed_settings[Setting::Theme] = theme;
    this->restart_required = true;
  });

  connect(ui->startOnBoot, &QCheckBox::clicked, this, [this](bool checked) {
    this->changed_settings[Setting::StartOnBoot] = checked;

    if (checked) {
      // TODO
    }
  });

  connect(ui->checkForUpdates, &QCheckBox::clicked, this,
          [this](bool checked) { this->changed_settings[Setting::CheckForUpdates] = checked; });

  connect(ui->startMinimized, &QCheckBox::clicked, this,
          [this](bool checked) { this->changed_settings[Setting::StartMinimized] = checked; });

  connect(ui->minimizeToTray, &QCheckBox::clicked, this,
          [this](bool checked) { this->changed_settings[Setting::MinimizeToTray] = checked; });

  connect(ui->closeToTray, &QCheckBox::clicked, this,
          [this](bool checked) { this->changed_settings[Setting::CloseToTray] = checked; });
}  // namespace Views

Application::~Application() {
  delete ui;
}

void Application::resetToDefault() {
  Settings s;

  auto language = s.getDefault(Setting::Language).toString();
  auto startOnBoot = s.getDefault(Setting::StartOnBoot).toBool();
  auto checkForUpdates = s.getDefault(Setting::CheckForUpdates).toBool();
  auto startMinimized = s.getDefault(Setting::StartMinimized).toBool();
  auto minimizeToTray = s.getDefault(Setting::MinimizeToTray).toBool();
  auto closeToTray = s.getDefault(Setting::CloseToTray).toBool();
  auto theme = s.getDefault(Setting::Theme).toString();

  ui->language->setCurrentText(language);
  ui->startOnBoot->setChecked(startOnBoot);
  ui->checkForUpdates->setChecked(checkForUpdates);
  ui->startMinimized->setChecked(startMinimized);
  ui->minimizeToTray->setChecked(minimizeToTray);
  ui->closeToTray->setChecked(closeToTray);
  ui->theme->setCurrentText(theme);
}

#ifdef Q_OS_WIN
const QString boot_key = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run";
#endif

void Application::commit() {
  if (ui->startOnBoot->isChecked()) {
#ifdef Q_OS_WIN
    QSettings reg(boot_key, QSettings::NativeFormat);
    reg.setValue("Shinjiru", "\"" + qApp->applicationFilePath().replace("/", "\\") + "\"");
#endif
  } else {
#ifdef Q_OS_WIN
    QSettings reg(boot_key, QSettings::NativeFormat);
    reg.remove("Shinjiru");
#endif
  }
}
}  // namespace Views
