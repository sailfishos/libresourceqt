# 
# Do NOT Edit the Auto-generated Part!
# Generated by: spectacle version 0.26
# 

Name:       libresourceqt-qt5

# >> macros
# << macros

Summary:    Resource Policy Qt API
Version:    1.26.0.1
Release:    1
Group:      System/Resource Policy
License:    LGPLv2.1
URL:        https://github.com/mer-packages/libresourceqt
Source0:    libresourceqt-1.26+master7c4a607.tar.bz2
Source100:  libresourceqt-qt5.yaml
Patch0:     0001-Don-t-cast-Resource-object-s-pointer-to-identifier.patch
Patch1:     gcc47_include_unistd.patch
Requires:   libdbus-qeventloop-qt5 = %{version}-%{release}
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Test)
BuildRequires:  pkgconfig(Qt5DBus)
BuildRequires:  pkgconfig(Qt5Gui)
BuildRequires:  pkgconfig(libresource)
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
%setup -q -n libresourceqt-1.26+master7c4a607

# 0001-Don-t-cast-Resource-object-s-pointer-to-identifier.patch
%patch0 -p1
# gcc47_include_unistd.patch
%patch1 -p1
# >> setup
# << setup

%build
# >> build pre
# << build pre

%qmake5 

make %{?jobs:-j%jobs}

# >> build post
# << build post

%install
rm -rf %{buildroot}
# >> install pre
# << install pre
%qmake_install

# >> install post
# << install post

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%post -n libdbus-qeventloop-qt5 -p /sbin/ldconfig

%postun -n libdbus-qeventloop-qt5 -p /sbin/ldconfig

%post -n libmediaoverridesqt5 -p /sbin/ldconfig

%postun -n libmediaoverridesqt5 -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%{_libdir}/libresourceqt5.so.*
# >> files
# << files

%files devel
%defattr(-,root,root,-)
%{_includedir}/resource/qt5/policy/*resource*.h
%{_libdir}/libresourceqt5.so
%{_libdir}/pkgconfig/libresourceqt5.pc
# >> files devel
# << files devel

%files -n libdbus-qeventloop-qt5
%defattr(-,root,root,-)
%{_libdir}/libdbus-qeventloop-qt5.so.*
# >> files libdbus-qeventloop-qt5
# << files libdbus-qeventloop-qt5

%files -n libdbus-qeventloop-qt5-devel
%defattr(-,root,root,-)
%{_includedir}/dbusconnectioneventloop.h
%{_libdir}/libdbus-qeventloop-qt5.so
%{_libdir}/pkgconfig/libdbus-qeventloop5.pc
# >> files libdbus-qeventloop-qt5-devel
# << files libdbus-qeventloop-qt5-devel

%files -n libmediaoverridesqt5
%defattr(-,root,root,-)
%{_libdir}/libmediaoverridesqt5.so.*
# >> files libmediaoverridesqt5
# << files libmediaoverridesqt5

%files -n libmediaoverridesqt5-devel
%defattr(-,root,root,-)
%{_includedir}/resource/qt5/policy/override.h
%{_libdir}/libmediaoverridesqt5.so
%{_libdir}/pkgconfig/libmediaoverridesqt5.pc
# >> files libmediaoverridesqt5-devel
# << files libmediaoverridesqt5-devel

%files client
%defattr(-,root,root,-)
%{_bindir}/resourceqt5-client
# >> files client
# << files client

%files tests
%defattr(-,root,root,-)
%{_libdir}/libresourceqt-qt5-tests/*
%{_datadir}/%{name}-tests/tests.xml
%config %{_sysconfdir}/dbus-1/system.d/test-dbus-qeventloop.conf
# >> files tests
# << files tests
