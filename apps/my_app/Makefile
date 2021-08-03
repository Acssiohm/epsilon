# apps += My::App
# app_headers += apps/my_app/app.h

app_my_src = $(addprefix apps/my_app/,\
  app.cpp \
  controller.cpp \
)

apps_src += $(app_my_src)

i18n_files += $(call i18n_for_locale,my_app/base,universal)

# $(eval $(call depends_on_image,apps/my_app/app.cpp,apps/my_app/my_app.png))
