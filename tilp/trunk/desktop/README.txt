This directory contains FreeDesktop.org Desktop and MIME types definitions for TILP.

If your desktop environment doesn't detect the files after `make install`, run:
* `update-desktop-database`;
* `update-mime-database [YOUR_INSTALL_PREFIX, e.g. /usr]/share/mime > /dev/null`.

This also now contains an AppData XML file, that can be validated using the
tool `appstream-util validate [PREFIX]/share/appdata/tilp.appdata.xml`.

See https://secure.freedesktop.org/~hughsient/appdata/ for more on the
specification.
