Name:           webmonx
Version:        0.3.0
%if 0%{?fedora_version} >= 8
Release:        0%{?dist}
%else
Release:        0
%endif
Summary:        Website update monitoring tool
Vendor:         Felix Geyer


%if 0%{?suse_version} >= 1000
Group:          Productivity/Networking/Web/Utilities
%endif
%if 0%{?fedora_version} >= 8
Group:          Applications/Internet
%endif
License:        GPL
URL:            http://sniperbeamer.de/webmonx/
Source0:        %{name}-%{version}.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{version}-build

%if 0%{?suse_version} >= 1000
BuildRequires:  libqt4-devel >= 4.3.0, update-desktop-files
Requires:       libqt4 >= 4.3.0, libqt4-x11 >= 4.3.0
%endif
%if 0%{?fedora_version} >= 8
BuildRequires:  qt4-devel >= 4.3.0, desktop-file-utils, gcc-c++
Requires:       qt4 >= 4.3.0, qt4-x11 >= 4.3.0
Requires(post): desktop-file-utils
Requires(postun): desktop-file-utils
%endif

%description
WebMonX monitors websites and notifies the user about updates.
It is a cross-platform port of WebMon.


%prep
%setup -q


%build
%if 0%{?suse_version} >= 1000
qmake PREFIX=$RPM_BUILD_ROOT%{_prefix}
%endif
%if 0%{?fedora_version} >= 8
qmake-qt4 PREFIX=$RPM_BUILD_ROOT%{_prefix}
%endif
make


%install
%if 0%{?fedora_version} >= 8
rm -rf $RPM_BUILD_ROOT
%endif
make install
%if 0%{?suse_version} >= 1000
%suse_update_desktop_file -r $RPM_BUILD_ROOT%{_prefix}/share/applications/webmonx.desktop "Network Monitor"
%endif


%clean
rm -rf $RPM_BUILD_ROOT


%if 0%{?fedora_version} >= 8
%post
touch --no-create %{_datadir}/icons/hicolor || :
%{_bindir}/gtk-update-icon-cache --quiet %{_datadir}/icons/hicolor || :
 
%postun
touch --no-create %{_datadir}/icons/hicolor || :
%{_bindir}/gtk-update-icon-cache --quiet %{_datadir}/icons/hicolor || :
%endif


%files
%defattr(-,root,root)
%{_prefix}/bin/webmonx
%{_prefix}/share/applications/webmonx.desktop
%{_prefix}/share/apps/webmonx/icons/*/*.png
%{_prefix}/share/apps/webmonx/sounds/*.wav
%{_prefix}/share/icons/hicolor/*/apps/*.png
%{_prefix}/share/pixmaps/*.xpm


%changelog
* Thu Nov  1 2007 - Felix Geyer <sniperbeamer_pkg@fobos.de> - 0.3.0-0
- Update to 0.3.0
* Fri Apr  6 2007 - Felix Geyer <sniperbeamer_pkg@fobos.de> - 0.2.0-0
- Update to 0.2.0
* Fri Jan  5 2007 - Felix Geyer <sniperbeamer_pkg@fobos.de> - 0.1.0-0
- Initial release
