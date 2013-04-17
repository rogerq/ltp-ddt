#
# schedutils RPM Spec File - 27 April 2002
#

Summary: Utilities for manipulating process scheduler attributes
Name: schedutils
Version: 1.4.0
Release: 1
License: GPL
Group: Applications/System
Source: schedutils-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-root

%description
schedutils is a set of utilities for retrieving and manipulating process
scheduler-related attributes, such as real-time parameters and CPU affinity.

This package includes the chrt and taskset utilities.

Install this package if you need to set or get scheduler-related attributes.

%prep
%setup -q

%build
make CC="gcc $RPM_OPT_FLAGS"

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}/usr/bin %{buildroot}%{_mandir}/man1
make PREFIX=%{buildroot}/usr DOCDIR=$RPM_BUILD_ROOT%{_docdir}/schedutils \
	MANDIR=$RPM_BUILD_ROOT%{_mandir} install

%clean
rm -rf %{buildroot}

%files
%defattr(0644, root, root, 755)
%doc AUTHORS ChangeLog COPYING README
%attr(555,root,root) %{_bindir}/chrt
%attr(555,root,root) %{_bindir}/taskset
%attr(0644,root,root) %{_mandir}/man1/chrt.1.gz
%attr(0644,root,root) %{_mandir}/man1/taskset.1.gz

%changelog
* Sat Aug 09 2003 Florian La Roche <Florian.LaRoche@redhat.de>
- fix 'Group' field

* Sun Jan 19 2003 Robert Love <rml@tech9.net>
- remove irqset.sh and irqset.1 as they are no longer in the package

* Tue Dec 31 2002 Robert Love <rml@tech9.net>
- remove lsrt and lsrt.1 as they are no longer in the package

* Wed Dec 11 2002 Robert Love <rml@tech9.net>
- lots of little updates

* Sun Apr 28 2002 Robert Love <rml@tech9.net>
- initial packaging of 0.0.1
