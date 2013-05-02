# 
# Do NOT Edit the Auto-generated Part!
# Generated by: spectacle version 0.26
# 

Name:       libresourceqt

# >> macros
# << macros

Summary:    Resource Policy Qt API
Version:    1.26+masterbeda977
Release:    1
Group:      System/Resource Policy
License:    LGPLv2.1
URL:        http://meego.gitorious.org/maemo-multimedia/libresourceqt/
Source0:    %{name}-%{version}.tar.bz2
Source100:  libresourceqt.yaml
Patch0:     0001-Don-t-cast-Resource-object-s-pointer-to-identifier.patch
Requires:   libdbus-qeventloop = %{version}-%{release}
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
BuildRequires:  pkgconfig(QtCore) >= 4.6.0
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

%package -n libdbus-qeventloop
Summary:    Library that runs the low level D-Bus inside QEventloop
Group:      System/Resource Policy
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description -n libdbus-qeventloop
This library enables Qt applications to run the D-Bus low-level api in the Qt Eventloop.

%package -n libdbus-qeventloop-devel
Summary:    Development files for libdbus-qeventloop
Group:      Development/Libraries
Requires:   libdbus-qeventloop = %{version}-%{release}

%description -n libdbus-qeventloop-devel
Development files for libdbus-qeventloop.

%package -n libmediaoverridesqt
Summary:    Mediaoverridesqt library
Group:      System/Resource Policy
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description -n libmediaoverridesqt
%{summary}.


%package -n libmediaoverridesqt-devel
Summary:    Development files for libmediaoverridesqt1
Group:      Development/Libraries
Requires:   libmediaoverridesqt = %{version}-%{release}

%description -n libmediaoverridesqt-devel
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
Requires:   libdbus-qeventloop = %{version}-%{release}

%description tests
Unit-tests for %{name}.

%prep
%setup -q -n %{name}-%{version}
%patch0 -p1
# >> setup
# << setup

%build
# >> build pre
# << build pre

%qmake 

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

%post -n libdbus-qeventloop -p /sbin/ldconfig

%postun -n libdbus-qeventloop -p /sbin/ldconfig

%post -n libmediaoverridesqt -p /sbin/ldconfig

%postun -n libmediaoverridesqt -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%{_libdir}/libresourceqt.so.*
# >> files
# << files

%files devel
%defattr(-,root,root,-)
%{_includedir}/resource/qt4/policy/*resource*.h
%{_libdir}/libresourceqt.so
%{_libdir}/pkgconfig/libresourceqt1.pc
%doc %{_datadir}/doc/%{name}/*
# >> files devel
# << files devel

%files -n libdbus-qeventloop
%defattr(-,root,root,-)
%{_libdir}/libdbus-qeventloop.so.*
# >> files libdbus-qeventloop
# << files libdbus-qeventloop

%files -n libdbus-qeventloop-devel
%defattr(-,root,root,-)
%{_includedir}/dbusconnectioneventloop.h
%{_libdir}/libdbus-qeventloop.so
%{_libdir}/pkgconfig/libdbus-qeventloop1.pc
# >> files libdbus-qeventloop-devel
# << files libdbus-qeventloop-devel

%files -n libmediaoverridesqt
%defattr(-,root,root,-)
%{_libdir}/libmediaoverridesqt.so.*
# >> files libmediaoverridesqt
# << files libmediaoverridesqt

%files -n libmediaoverridesqt-devel
%defattr(-,root,root,-)
%{_includedir}/resource/qt4/policy/override.h
%{_libdir}/libmediaoverridesqt.so
%{_libdir}/pkgconfig/libmediaoverridesqt1.pc
# >> files libmediaoverridesqt-devel
# << files libmediaoverridesqt-devel

%files client
%defattr(-,root,root,-)
%{_bindir}/resourceqt-client
# >> files client
# << files client

%files tests
%defattr(-,root,root,-)
%{_libdir}/libresourceqt-tests/*
%{_datadir}/%{name}-tests/tests.xml
%config %{_sysconfdir}/dbus-1/system.d/test-dbus-qeventloop.conf
# >> files tests
# << files tests
