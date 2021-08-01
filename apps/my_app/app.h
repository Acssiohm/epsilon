#ifndef MY_APP_H
#define MY_APP_H
#include "content.h"
#include "../shared/shared_app.h"


namespace My {

class App : public ::App {
public:
  class Descriptor : public ::App::Descriptor {
  public:
    I18n::Message name() override;
    I18n::Message upperName() override;
    const Image * icon() override;
  };
  class Snapshot : public ::SharedApp::Snapshot {
  public:
    App * unpack(Container * container) override;
    Descriptor * descriptor() override;
  };
  TELEMETRY_ID("My");
private:
  App(Snapshot * snapshot);
  MyController m_main;
};


}

#endif
