Name:       libresourceqt-qt5
Summary:    Resource Policy Qt API
Version:    1.32.1
Release:    1
License:    LGPLv2
URL:        https://github.com/sailfishos/libresourceqt/
Source0:    %{name}-%{version}.tar.bz2
Requires:   libdbus-qeventloop-qt5 = %{version}-%{release}
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Test)
BuildRequires:  pkgconfig(Qt5DBus)
BuildRequires:  pkgconfig(Qt5Gui)
BuildRequires:  pkgconfig(libresource) >= 0.25.0
BuildRequires:  pkgconfig(dbus-1)

%description
Qt library for Resource Policy.

%package devel
Summary:    Development files for %{name}
Requires:   %{name} = %{version}-%{release}

%description devel
Development files for %{name}.

%package -n libdbus-qeventloop-qt5
Summary:    Library that runs the low level D-Bus inside QEventloop
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description -n libdbus-qeventloop-qt5
This library enables Qt applications to run the D-Bus low-level api in the Qt Eventloop.

%package -n libdbus-qeventloop-qt5-devel
Summary:    Development files for libdbus-qeventloop-qt5
Requires:   libdbus-qeventloop-qt5 = %{version}-%{release}

%description -n libdbus-qeventloop-qt5-devel
Development files for libdbus-qeventloop-qt5.

%package -n libmediaoverridesqt5
Summary:    Mediaoverridesqt5 library
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description -n libmediaoverridesqt5
%{summary}.


%package -n libmediaoverridesqt5-devel
Summary:    Development files for libmediaoverridesqt1
Requires:   libmediaoverridesqt5 = %{version}-%{release}

%description -n libmediaoverridesqt5-devel
%{summary}.


%package client
Summary:    Test client to test %{name}
Requires:   %{name} = %{version}-%{release}

%description client
Test client to test %{name}.

%package tests
Summary:    Unit-tests for %{name}
Requires:   %{name} = %{version}-%{release}
Requires:   libdbus-qeventloop-qt5 = %{version}-%{release}

%description tests
Unit-tests for %{name}.

%prep
%setup -q -n %{name}-%{version}

%build
%qmake5 "VERSION=%{version}"
%make_build

%install
%qmake5_install

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%post -n libdbus-qeventloop-qt5 -p /sbin/ldconfig

%postun -n libdbus-qeventloop-qt5 -p /sbin/ldconfig

%post -n libmediaoverridesqt5 -p /sbin/ldconfig

%postun -n libmediaoverridesqt5 -p /sbin/ldconfig

%files
%{_libdir}/libresourceqt5.so.*
%license COPYING

%files devel
%{_includedir}/resource/qt5/policy/*resource*.h
%{_libdir}/libresourceqt5.so
%{_libdir}/pkgconfig/libresourceqt5.pc

%files -n libdbus-qeventloop-qt5
%{_libdir}/libdbus-qeventloop-qt5.so.*

%files -n libdbus-qeventloop-qt5-devel
%{_includedir}/dbusconnectioneventloop.h
%{_libdir}/libdbus-qeventloop-qt5.so
%{_libdir}/pkgconfig/libdbus-qeventloop5.pc

%files -n libmediaoverridesqt5
%{_libdir}/libmediaoverridesqt5.so.*

%files -n libmediaoverridesqt5-devel
%{_includedir}/resource/qt5/policy/override.h
%{_libdir}/libmediaoverridesqt5.so
%{_libdir}/pkgconfig/libmediaoverridesqt5.pc

%files client
%{_bindir}/resourceqt5-client

%files tests
%{_libdir}/libresourceqt-qt5-tests/
%{_datadir}/%{name}-tests/tests.xml
