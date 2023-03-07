!template load file=app_srs.md.template app=BlackBear category=blackbear

!template! item key=system-scope
!! system-scope-begin

BlackBear models the effects of environmental exposure on structures. These environmental conditions that can induce aging include evolving mechanical loading and exposure to temperature and moisture. The materials modeled by BlackBear include those typically used in the construction of civil structures, such as concrete and steel. Under normal service, these structures can experience a variety of degradation mechanisms. For example, steel and other metals can degrade due to corrosion, embrittlement, and stress corrosion cracking. Concrete can experience degradation due to expansive reactions, chemical attacks, leaching, and mechanical mechanisms induced by conditions such as freeze/thaw cycles and fatigue.

In addition to modeling the degradation mechanisms, BlackBear also models the response of these structures to a variety of loading conditions that these structures are expected to safely withstand. These include a variety of normal thermal and mechanical service loading conditions, as well as wind and earthquake loading conditions. BlackBear includes models to represent the behavior of structural materials under those conditions, which can induce creep, plasticity, damage, fracture, and failure of these materials.

!! system-scope-finish
!template-end!

!template! item key=system-purpose
!! system-purpose-begin
The purpose of BlackBear is to simulate the progression of degradation in structural components (e.g., buildings, bridges, dams) due to exposure to the environmental conditions they experience over their lifetime. In addition, BlackBear predicts the response of those structures in the presence of that long-term degradation to loading conditions of interest to assess the effects of degradation on their integrity.

!! system-purpose-finish
!template-end!

!template! item key=assumptions-and-dependencies
{{app}} has no constraints on hardware and software beyond those of the MOOSE framework and modules listed in their respective SRS documents, which are accessible through the links at the beginning of this document.

{{app}} provides access to a number of code objects that perform computations such as material behavior and boundary conditions. These objects each of makes their own physics-based assumptions, such as the units of the inputs and outputs. Those assumptions are described in the documentation for those individual objects.
!template-end!

!template! item key=user-characteristics
{{app}} has three main classes of users:

- +{{app}} Developers+: These are the core developers of {{app}} module. They are responsible for designing, implementing, and maintaining the software, while following and enforcing its software development standards.
- +Developers+: These are scientists or engineers that modify or add capabilities to {{app}} for their own purposes, which may include research or extending its capabilities. They will typically have a background in structural or mechanical engineering, and in modeling and simulation techniques, but may have more limited background in code development using the C++ language. In many cases, these developers will be encouraged to contribute code back to {{app}}.
- +Analysts+: These are users that run {{app}} to run simulations, but do not develop code. The primary interface of these users with {{app}} is the input files that define their simulations. These users may interact with developers of the system requesting new features and reporting bugs found.
!template-end!

!template item key=reliability
The regression test suite will cover at least 90% of all lines of code at all times. Known
regressions will be recorded and tracked (see [#maintainability]) to an independent and
satisfactory resolution.
