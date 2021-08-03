#include "app.h"

#include "my_app.h"

extern "C" {
#include <assert.h>
}

namespace My {

I18n::Message App::Descriptor::name() {
  return I18n::Message::MyApp;
}

I18n::Message App::Descriptor::upperName() {
  return I18n::Message::MyCapApp;
}

const Image * App::Descriptor::icon() {
  return ImageStore::MyApp;
}

App * App::Snapshot::unpack(Container * container) {
  return new (container->currentAppBuffer()) App(this);
}

App::Descriptor * App::Snapshot::descriptor() {
  static Descriptor descriptor;
  return &descriptor;
}

App::App(Snapshot * snapshot) :
  ::App(snapshot, &m_main, I18n::Message::Warning),
  m_main(&m_modalViewController, snapshot, &m_tab1),
  m_tab1(&m_main)
{
}

}
