apps += My::App
app_headers += apps/my_app/app.h

app_my_src = $(addprefix apps/my_app/,\
  app.cpp \
)

app_my_src += $(app_my_test_src)
apps_src += $(app_my_src)

i18n_files += my_app/base.universal.i18n

$(eval $(call depends_on_image,apps/my_app/app.cpp,apps/my_app/my_app.png))
