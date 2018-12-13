Name:       libresourceqt-qt5
Summary:    Resource Policy Qt API
Version:    1.29
Release:    1
Group:      System/Resource Policy
License:    LGPLv2.1
URL:        http://meego.gitorious.org/maemo-multimedia/libresourceqt/
Source0:    %{name}-%{version}.tar.bz2
Requires:   libdbus-qeventloop-qt5 = %{version}-%{release}
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Test)
BuildRequires:  pkgconfig(Qt5DBus)
BuildRequires:  pkgconfig(Qt5Gui)
BuildRequires:  pkgconfig(libresource) >= 0.23.0
BuildRequires:  pkgconfig(dbus-1)

%description
Qt library for Resource Policy.

%package devel
Summary:    Development files for %{name}
Group:      Development/Libraries
Requires:   %{name} = %{version}-%{release}

%description devel
Development files for %{name}.

%package -n libdbus-qeventloop-qt5
Summary:    Library that runs the low level D-Bus inside QEventloop
Group:      System/Resource Policy
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description -n libdbus-qeventloop-qt5
This library enables Qt applications to run the D-Bus low-level api in the Qt Eventloop.

%package -n libdbus-qeventloop-qt5-devel
Summary:    Development files for libdbus-qeventloop-qt5
Group:      Development/Libraries
Requires:   libdbus-qeventloop-qt5 = %{version}-%{release}

%description -n libdbus-qeventloop-qt5-devel
Development files for libdbus-qeventloop-qt5.

%package -n libmediaoverridesqt5
Summary:    Mediaoverridesqt5 library
Group:      System/Resource Policy
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description -n libmediaoverridesqt5
%{summary}.


%package -n libmediaoverridesqt5-devel
Summary:    Development files for libmediaoverridesqt1
Group:      Development/Libraries
Requires:   libmediaoverridesqt5 = %{version}-%{release}

%description -n libmediaoverridesqt5-devel
%{summary}.


%package client
Summary:    Test client to test %{name}
Group:      Development/Tools
Requires:   %{name} = %{version}-%{release}

%description client
Test client to test %{name}.

%package tests
Summary:    Unit-tests for %{name}
Group:      System/Resource Policy
Requires:   %{name} = %{version}-%{release}
Requires:   libdbus-qeventloop-qt5 = %{version}-%{release}

%description tests
Unit-tests for %{name}.

%prep
%setup -q -n %{name}-%{version}

%build
%qmake5 "VERSION=%{version}"
make %{?_smp_mflags}

%install
rm -rf %{buildroot}
%qmake5_install

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%post -n libdbus-qeventloop-qt5 -p /sbin/ldconfig

%postun -n libdbus-qeventloop-qt5 -p /sbin/ldconfig

%post -n libmediaoverridesqt5 -p /sbin/ldconfig

%postun -n libmediaoverridesqt5 -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%{_libdir}/libresourceqt5.so.*

%files devel
%defattr(-,root,root,-)
%{_includedir}/resource/qt5/policy/*resource*.h
%{_libdir}/libresourceqt5.so
%{_libdir}/pkgconfig/libresourceqt5.pc

%files -n libdbus-qeventloop-qt5
%defattr(-,root,root,-)
%{_libdir}/libdbus-qeventloop-qt5.so.*

%files -n libdbus-qeventloop-qt5-devel
%defattr(-,root,root,-)
%{_includedir}/dbusconnectioneventloop.h
%{_libdir}/libdbus-qeventloop-qt5.so
%{_libdir}/pkgconfig/libdbus-qeventloop5.pc

%files -n libmediaoverridesqt5
%defattr(-,root,root,-)
%{_libdir}/libmediaoverridesqt5.so.*

%files -n libmediaoverridesqt5-devel
%defattr(-,root,root,-)
%{_includedir}/resource/qt5/policy/override.h
%{_libdir}/libmediaoverridesqt5.so
%{_libdir}/pkgconfig/libmediaoverridesqt5.pc

%files client
%defattr(-,root,root,-)
%{_bindir}/resourceqt5-client

%files tests
%defattr(-,root,root,-)
%{_libdir}/libresourceqt-qt5-tests/*
%{_datadir}/%{name}-tests/tests.xml
