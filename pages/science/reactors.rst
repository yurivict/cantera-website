.. slug: reactors
.. title: Reactor Models in Cantera
.. has_math: true

.. jumbotron::

   .. raw:: html

      <h1 class="display-3">Reactors and Reactor Networks</h1>

   .. class:: lead

      Cantera Reactors and Reactor Networks model zero-dimensional reactors and their
      interactions with the surroundings.

Reactors
========

A Cantera :py:class:`Reactor` represents the simplest form of a chemically reacting system. It
corresponds to an extensive thermodynamic control volume :math:`V`, in which all state variables are
homogeneously distributed. The system is generally unsteady -- that is, all states are functions of time.
In particular, transient state changes due to chemical reactions are possible. However,
thermodynamic (but not chemical) equilibrium is assumed to be present throughout the reactor at all
instants of time.

Reactors can interact with the surrounding environment in multiple ways:

- Expansion/compression work: By moving the walls of the reactor, its volume can be changed and
  expansion or compression work can be done by or on the reactor.

- Heat transfer: An arbitrary heat transfer rate can be defined to cross the boundaries of the
  reactor.

- Mass transfer: The reactor can have multiple inlets and outlets. For the inlets, arbitrary states
  can be defined. Fluid with the current state of the reactor exits the reactor at the outlets.

- Surface interaction: One or multiple walls can influence the chemical reactions in the reactor.
  This is not just restricted to catalytic reactions, but mass transfer between the surface and the
  fluid can also be modeled.

All of these interactions do not have to be constant, but can vary as a function of time or state.
For example, heat transfer can be described as a function of the temperature difference between the
reactor and the environment, or the wall movement can be modeled depending on the pressure
difference. Interactions of the reactor with the environment are defined on one or more *walls*,
*inlets*, and *outlets*.

In addition to single reactors, Cantera is also able to interconnect reactors into a Reactor
Network. Each reactor in a network may be connected so that the contents of one reactor flow into
another. Reactors may also be in contact with one another or the environment via walls that conduct
heat or move to do work.

Governing Equations for Single Reactors
=======================================

The state variables for Cantera's general reactor model are

- :math:`m`, the mass of the reactor's contents (in kg)

- :math:`V`, the reactor volume (in m\ :sup:`3`) (not a state variable for
  :py:class:`ConstPressureReactor` and :py:class:`IdealGasConstPressureReactor`)

- A state variable describing the energy of the system, depending on the
  configuration (see `Energy Conservation`_ for further explanation):

  - General :py:class:`Reactor`: :math:`U`, the total internal energy of the reactors
    contents (in J)

  - :py:class:`ConstPressureReactor`: :math:`H`, the total enthalpy of the reactors
    contents (in J)

  - :py:class:`IdealGasReactor` and :py:class:`IdealGasConstPressureReactor`: :math:`T`,
    the temperature (in K)

- :math:`Y_k`, the mass fractions for each species (dimensionless)

Mass Conservation
-----------------

The total mass of the reactor's contents changes as a result of flow through
the reactor's inlets and outlets, and production of homogeneous phase species
on the reactor walls:

.. math::

   \frac{dm}{dt} = \sum_{in} \dot{m}_{in} - \sum_{out} \dot{m}_{out} +
                    \dot{m}_{wall}

Species Conservation
--------------------

The rate at which species :math:`k` is generated through homogeneous phase
reactions is :math:`V \dot{\omega}_k W_k`, and the total rate at which species
:math:`k` is generated is:

.. math::

   \dot{m}_{k,gen} = V \dot{\omega}_k W_k + \dot{m}_{k,wall}

The rate of change in the mass of each species is:

.. math::

   \frac{d(mY_k)}{dt} = \sum_{in} \dot{m}_{in} Y_{k,in} -
                         \sum_{out} \dot{m}_{out} Y_k +
                         \dot{m}_{k,gen}

Expanding the derivative on the left hand side and substituting the equation
for :math:`dm/dt`, the equation for each homogeneous phase species is:

.. math::

   m \frac{dY_k}{dt} = \sum_{in} \dot{m}_{in} (Y_{k,in} - Y_k)+
                      \dot{m}_{k,gen} - Y_k \dot{m}_{wall}


Reactor Volume
--------------

The reactor volume changes as a function of time due to the motion of one or
more walls:

.. math::

   \frac{dV}{dt} = \sum_w f_w A_w v_w(t)

where :math:`f_w = \pm 1` indicates the facing of the wall (whether moving the wall increases or
decreases the volume of the reactor), :math:`A_w` is the
surface area of the wall, and :math:`v_w(t)` is the velocity of the wall as a
function of time.

For the :py:class:`ConstPressureReactor` and :py:class:`IdealGasConstPressureReactor`, the
volume is not a state variable, but instead takes on whatever value is
consistent with holding the pressure constant.

Energy Conservation
-------------------

The solution of the energy equation can be enabled or disabled by changing the
``energy_enabled`` flag. It is enabled by default.

The implemented formulation of the energy equation depends on which reactor
model is used.

Standard Reactor
****************

The equation for the total internal energy is found by writing the first law
for an open system:

.. math::

   \frac{dU}{dt} = - p \frac{dV}{dt} + \dot{Q} +
                    \sum_{in} \dot{m}_{in} h_{in} - h \sum_{out} \dot{m}_{out}

where :math:`\dot{Q}` is the net rate of heat addition to the system. [1]_

Constant Pressure Reactor
*************************

For this reactor model, the pressure is held constant. The volume is not a
state variable, but instead takes on whatever value is consistent with holding
the pressure constant. The total enthalpy replaces the total internal energy
as a state variable. Using the definition of the total enthalpy:

.. math::

   H = U + pV

   \frac{d H}{d t} = \frac{d U}{d t} + p \frac{dV}{dt} + V \frac{dp}{dt}

Noting that :math:`dp/dt = 0` and substituting into the energy equation yields:

.. math::

   \frac{dH}{dt} = \dot{Q} + \sum_{in} \dot{m}_{in} h_{in}
                   - h \sum_{out} \dot{m}_{out}


Ideal Gas Reactor
*****************

In case of the Ideal Gas Reactor Model, the reactor temperature :math:`T` is
used instead of the total internal energy :math:`U` as a state variable. For an
ideal gas, we can rewrite the total internal energy in terms of the mass
fractions and temperature:

.. math::

   U = m \sum_k Y_k u_k(T)

   \frac{dU}{dt} = u \frac{dm}{dt}
                   + m c_v \frac{dT}{dt}
                   + m \sum_k u_k \frac{dY_k}{dt}

Substituting the corresponding derivatives yields an equation for the
temperature:

.. math::

   m c_v \frac{dT}{dt} = - p \frac{dV}{dt} + \dot{Q}
       + \sum_{in} \dot{m}_{in} \left( h_{in} - \sum_k u_k Y_{k,in} \right)
       - \frac{p V}{m} \sum_{out} \dot{m}_{out} - \sum_k \dot{m}_{k,gen} u_k

While this form of the energy equation is somewhat more complicated, it
significantly reduces the cost of evaluating the system Jacobian, since the
derivatives of the species equations are taken at constant temperature instead
of constant internal energy.

Ideal Gas Constant Pressure Reactor
***********************************

As for the Ideal Gas Reactor, we replace the total enthalpy as a state
variable with the temperature by writing the total enthalpy in terms of the
mass fractions and temperature:

.. math::

   H = m \sum_k Y_k h_k(T)

   \frac{dH}{dt} = h \frac{dm}{dt} + m c_p \frac{dT}{dt}
                   + m \sum_k h_k \frac{dY_k}{dt}

Substituting the corresponding derivatives yields an equation for the
temperature:

.. math::

   m c_p \frac{dT}{dt} = \dot{Q} - \sum_k h_k \dot{m}_{k,gen}
       + \sum_{in} \dot{m}_{in} \left(h_{in} - \sum_k h_k Y_{k,in} \right)

Wall Interactions
-----------------

Walls are stateless objects in Cantera, meaning that no differential equation
is integrated to determine any wall property. Since it is the wall (piston)
velocity that enters the energy equation, this means that it is the velocity,
not the acceleration or displacement, that is specified. The wall velocity is
computed from

.. math::

   v = K(P_{\mathrm{left}} - P_{\mathrm{right}}) + v_0(t),

where :math:`K` is a non-negative constant, and :math:`v_0(t)` is a specified
function of time. The velocity is positive if the wall is moving to the right.

The total rate of heat transfer through all walls is:

.. math::

   \dot{Q} = \sum_w f_w \dot{Q}_w

where :math:`f_w = \pm 1` indicates the facing of the wall (-1 for the reactor
on the left, +1 for the reactor on the right). The heat flux :math:`\dot{Q}_w`
through a wall :math:`w` connecting reactors "left" and "right" is computed as:

.. math::

   \dot{Q}_w = U A (T_{\mathrm{left}} - T_{\mathrm{right}})
             + \epsilon\sigma A (T_{\mathrm{left}}^4 - T_{\mathrm{right}}^4)
             + A q_0(t)

where :math:`U` is a user-specified heat transfer coefficient (W/m\ :sup:`2`-K),
:math:`A` is the wall area (m\ :sup:`2`), :math:`\epsilon` is the user-specified
emissivity, :math:`\sigma` is the Stefan-Boltzmann radiation constant, and
:math:`q_0(t)` is a user-specified, time-dependent heat flux (W/m\ :sup:`2`).
This definition is such that positive :math:`q_0(t)` implies heat transfer from
the "left" reactor to the "right" reactor. Each of the user-specified terms
defaults to 0.

In case of surface reactions, there can be a net generation (or destruction) of
homogeneous (gas) phase species at the wall. The molar rate of production for
each homogeneous phase species :math:`k` on wall :math:`w` is
:math:`\dot{s}_{k,w}` (in kmol/s/m\ :sup:`2`). The total (mass) production rate
for homogeneous phase species :math:`k` on all walls is:

.. math::

   \dot{m}_{k,wall} = W_k \sum_w A_w \dot{s}_{k,w}

where :math:`W_k` is the molecular weight of species :math:`k` and :math:`A_w`
is the area of each wall. The net mass flux from all walls is then:

.. math::

   \dot{m}_{wall} = \sum_k \dot{m}_{k,wall}

For each surface species :math:`i`, the rate of change of the site fraction
:math:`\theta_{i,w}` on each wall :math:`w` is integrated with time:

.. math::

   \frac{d\theta_{i,w}}{dt} = \frac{\dot{s}_{i,w} n_i}{\Gamma_w}

where :math:`\Gamma_w` is the total surface site density on wall :math:`w` and
:math:`n_i` is the number of surface sites occupied by a molecule of species
:math:`i` (sometimes referred to within Cantera as the molecule's "size").

Reactor Networks and Devices
============================

While reactors by themselves just define the above governing equations of the
reactor, the time integration is performed in reactor networks. A reactor
network is therefore necessary even if only a single reactor is considered.

The advantage of reactor networks obviously is that multiple reactors can be
interconnected. Not only mass flow from one reactor into another can be
realized, but also heat can be transferred, or the wall between reactors can
move. To set up a network, the following components can be defined in addition
to the reactors previously mentioned:

- :py:class:`Reservoir`: A reservoir can be thought of as an infinitely large volume, in
  which all states are predefined and never change from their initial values.
  Typically, it represents a vessel to define temperature and composition of a
  stream of mass flowing into a reactor, or the ambient fluid surrounding the
  reactor network. Besides, the fluid flow finally finally exiting a reactor
  network has to flow into a reservoir. In the latter case, the state of the
  reservoir (except pressure) is irrelevant.

- :py:class:`Wall`: A wall separates two reactors, or a reactor and a reservoir. A wall
  has a finite area, may conduct or radiate heat between the two reactors on
  either side, and may move like a piston. See the `Wall Interactions`_ section for
  detail of how the wall affects the connected reactors.

- :py:class:`Valve`: A valve is a flow devices with mass flow rate that is a function of
  the pressure drop across it. The mass flow rate is computed as:

  .. math::

     \dot m = K_v g(t) f(P_1 - P_2)

  with :math:`K_v` being a proportionality constant that is set using the class
  property :py:func:`Valve.valve_coeff`. Further, :math:`g` and :math:`f`
  are functions of time and pressure drop that are set by class methods
  :py:func:`Valve.set_time_function` and :py:func:`Valve.set_valve_function`,
  respectively. If no functions are specified, the mass flow rate defaults to:

  .. math::

     \dot m = K_v (P_1 - P_2)

  The pressure difference between upstream (*1*) and downstream (*2*) reservoir
  is defined as :math:`P_1 - P_2`. It is never possible for the flow to reverse
  and go from the downstream to the upstream reactor/reservoir through a line
  containing a :py:class:`Valve` object, which means that the flow rate is set to zero if
  :math:`P_1 < P_2`.

  :py:class:`Valve` objects are often used between an upstream reactor and a downstream
  reactor or reservoir to maintain them both at nearly the same pressure. By
  setting the constant :math:`K_v` to a sufficiently large value, very small
  pressure differences will result in flow between the reactors that counteracts
  the pressure difference.

- :py:class:`MassFlowController`: A mass flow controller maintains a specified mass
  flow rate independent of upstream and downstream conditions. The equation used
  to compute the mass flow rate is

  .. math::

     \dot m = m_0 g(t)

  where :math:`m_0` is a mass flow coefficient and :math:`g` is a function of time
  which are set by class property :py:func:`MassFlowController.mass_flow_coeff`
  and method :py:func:`MassFlowController.set_time_function`, respectively. If no
  function is specified, the mass flow rate defaults to:

  .. math::

     \dot m = m_0

  Note that if :math:`\dot m < 0`, the mass flow rate will be set to zero,
  since a reversal of the flow direction is not allowed.

  Unlike a real mass flow controller, a :py:class:`MassFlowController` object will maintain
  the flow even if the downstream pressure is greater than the upstream
  pressure. This allows simple implementation of loops, in which exhaust gas
  from a reactor is fed back into it through an inlet. But note that this
  capability should be used with caution, since no account is taken of the work
  required to do this.

- :py:class:`PressureController`: A pressure controller is designed to be used in
  conjunction with another 'master' flow controller, typically a
  :py:class:`MassFlowController`. The master flow controller is installed on the inlet of
  the reactor, and the corresponding :py:class:`PressureController` is installed on on
  outlet of the reactor. The :py:class:`PressureController` mass flow rate is equal to the
  master mass flow rate, plus a small correction dependent on the pressure
  difference:

  .. math::

     \dot m = \dot m_{\text{master}} + K_v f(P_1 - P_2)

  where :math:`K_v` is a proportionality constant and :math:`f` is a function of
  pressure drop :math:`P_1 - P_2` that are set by class property
  :py:func:`PressureController.pressure_coeff` and method
  :py:func:`PressureController.set_pressure_function`, respectively. If no
  function is specified, the mass flow rate defaults to:

  .. math::

     \dot m = \dot m_{\text{master}} + K_v (P_1 - P_2)

  Note that if :math:`\dot m < 0`, the mass flow rate will be set to zero,
  since a reversal of the flow direction is not allowed.

Time Integration
----------------

Cantera uses the CVODES solver from the
`SUNDIALS <https://computing.llnl.gov/projects/sundials>`__
package to integrate the stiff ODEs of reacting systems. Starting off the
current state of the system, it can be advanced in time by one of the
following methods:

- ``step()``: The step method computes the state of the system at the a priori
  unspecified time :math:`t_{\mathrm{new}}`. The time :math:`t_{\mathrm{new}}`
  is internally computed so that all states of the system only change within a
  (specifiable) band of absolute and relative tolerances. Additionally, the time
  step must not be larger than a predefined maximum time step
  :math:`\Delta t_{\mathrm{max}}`. The new time :math:`t_{\mathrm{new}}` is
  returned by this function.

- ``advance(``\ :math:`t_{\mathrm{new}}`\ ``)``: This method computes the state of the
  system at time :math:`t_{\mathrm{new}}`. :math:`t_{\mathrm{new}}` describes
  the absolute time from the initial time of the system. By calling this method
  in a for loop for pre-defined times, the state of the system is obtained for
  exactly the times specified. Internally, several ``step()`` calls are
  typically performed to reach the accurate state at time
  :math:`t_{\mathrm{new}}`.

- ``advance_to_steady_state(max_steps, residual_threshold, atol,
  write_residuals)`` [Python interface only]: If the steady state solution of a
  reactor network is of interest, this method can be used. Internally, the
  steady state is approached by time stepping. The network is considered to be
  at steady state if the feature-scaled residual of the state vector is below a
  given threshold value (which by default is 10 times the time step ``rtol``).

The use of the ``advance`` method in a loop has the advantage that it produces
results corresponding to a predefined time series. These are associated with a
predefined memory consumption and well comparable between simulation runs with
different parameters. However, some detail (for example, a fast ignition process)
might not be resolved in the output data due to the typically large time steps.
To avoid losing this detail, the
`Reactor::setAdvanceLimit <{{% ct_docs doxygen/html/dc/d5e/classCantera_1_1Reactor.html#a9b630edc7d836e901886d7fd81134d9e %}}>`__
method (C++) or the :py:func:`Reactor.set_advance_limit` method (Python) can be
used to set the maximum amount that a specified solution component can change
between output times. For an example of this feature's use, see the example
`reactor1.py </examples/python/reactors/reactor1.py.html>`__.

The ``step`` method results in many more data points because of the small
timesteps needed. Additionally, the absolute time has to be kept track of
manually.



Even though Cantera comes pre-defined with typical parameters for tolerances
and the maximum internal time step, the solution sometimes diverges. To solve
this problem, three parameters can be tuned: The absolute time stepping
tolerances, the relative time stepping tolerances, and the maximum time step. A
reduction of the latter value is particularly useful when dealing with abrupt
changes in the boundary conditions (for example, opening/closing valves; see
also the `IC engine example </examples/python/reactors/ic_engine.py.html>`__).

General Usage in Cantera
========================

In Cantera, the following steps are typically necessary to investigate a
reactor network:

1. Define ``Solution`` objects for the fluids to be flowing through your reactor
   network.

2. Define the reactor type(s) and reservoir(s) that describe your system. Chose
   Ideal Gas (Constant Pressure) Reactor(s) if you only consider ideal gas
   phases.

3. *Optional:* Set up the boundary conditions and flow devices between reactors
   or reservoirs.

4. Define a reactor network which contains all the reactors previously created.

5. Advance the simulation in time, typically in a for- or while-loop. Note that
   only the current state is stored in Cantera by default. If you want to
   observe the transient states, you manually have to keep track of them.

6. Analyze the data.

Note that Cantera always solves a transient problem. If you are interested in steady-state
conditions, you can run your simulation for a long time until the states are converged (see the
`surface reactor example </examples/python/reactors/surf_pfr.py.html>`__ and the `combustor example
</examples/python/reactors/combustor.py.html>`__).

Cantera comes with a broad variety of well-commented example scrips for reactor
networks. Please see the `Cantera Examples </examples/index.html>`__ for further
information.

Common Reactor Types and their Implementation in Cantera
========================================================

Batch Reactor at Constant Volume or at Constant Pressure
--------------------------------------------------------

If you are interested in how a homogeneous chemical composition changes in time
when it is left to its own devices, a simple batch reactor can be used. Two versions
are commonly considered: A rigid vessel with fixed volume but variable
pressure, or a system idealized at constant pressure but varying volume.

In Cantera, such a simulation can be performed very easily. The initial state
of the solution can be specified by composition and a set of thermodynamic
parameters (like temperature and pressure) as a standard Cantera solution
object. Upon its base, a general (Ideal Gas) Reactor or an (Ideal Gas) Constant
Pressure Reactor can be created, depending on if a constant volume or constant
pressure batch reactor should be considered, respectively. The behavior of the
solution in time can be simulated as a very simple Reactor Network containing
only the formerly created reactor.

An example for such a Batch Reactor is given in the `examples
</examples/python/reactors/reactor1.py.html>`__.

Continuously Stirred Tank Reactor
---------------------------------

A Continuously Stirred Tank Reactor (CSTR), also often referred to as
Well-Stirred Reactor (WSR), Perfectly Stirred Reactor (PSR), or Longwell
Reactor, is essentially a single Cantera reactor with an inlet, an outlet, and
constant volume. Therefore, the `Governing Equations for Single Reactors`_
defined above apply accordingly.

Steady state solutions to CSTRs are often of interest. In this case, the mass
flow rate :math:`\dot{m}` is constant and equal at inlet and outlet. The mass
contained in the confinement :math:`m` divided by :math:`\dot{m}` defines the mean
residence time of the fluid in the confinement.

At steady state, the time derivatives in the governing equations become zero,
and the system of ordinary differential equations can be reduced to a set of
coupled nonlinear algebraic equations. A Newton solver could be used to solve
this system of equations. However, a sophisticated implementation might be
required to account for the strong nonlinearities and the presence of multiple
solutions.

Cantera does not have such a Newton solver implemented. Instead, steady CSTRs
are simulated by considering a time-dependent constant volume reactor with
specified in- and outflow conditions. Starting off at an initial solution, the
reactor network containing this reactor is advanced in time until the state of
the solution is converged. An example for this procedure is
`the combustor example </examples/python/reactors/combustor.py.html>`__.

A problem can be the ignition of a CSTR: If the reactants are not reactive
enough, the simulation can result in the trivial solution that inflow and
outflow states are identical. To solve this problem, the reactor can be
initialized with a high temperature and/or radical concentration. A good
approach is to use the equilibrium composition of the reactants (which can be
computed using Cantera's ``equilibrate`` function) as an initial guess.

Plug-Flow Reactor
-----------------

A Plug-Flow Reactor (PFR) represents a steady-state channel with a
cross-sectional area :math:`A`. Typically an ideal gas flows through it at a constant
mass flow rate :math:`\dot{m}`. Perpendicular to the flow direction, the gas is
considered to be completely homogeneous. In the axial direction :math:`z`, the states
of the gas is allowed to change. However, all diffusion processes are neglected.

Plug-Flow Reactors are often used to simulate ignition delay times, emission
formation, and catalytic processes.

The governing equations of Plug-Flow Reactors are [Kee2017]_:

- Mass conservation:

  .. math::

     \frac{d(\rho u A)}{dz} =  P' \sum_k \dot{s}_k W_k

  where :math:`u` is the axial velocity in (m/s) and :math:`P'` is the chemically active
  channel perimeter in m (chemically active perimeter per unit length).

- Continuity equation of species :math:`k`:

  .. math::

     \rho u \frac{d Y_k}{dz} + Y_k P' \sum_k \dot{s}_k W_k =
     \dot{\omega}_k W_k + P' \dot{s}_k W_k

- Energy conservation:

  .. math::

     \rho u A c_p \frac{d T}{d z} =
     - A \sum_k h_k \dot{\omega}_k W_k
     - P' \sum_k h_k \dot{s}_k W_k
     + U P (T_w - T)

  where :math:`U` is the heat transfer coefficient in W/m/K, :math:`P` is the perimeter of
  the duct in m, and :math:`T_w` is the wall temperature in K. Kinetic and
  potential energies are neglected.

- Momentum conservation in the axial direction:

  .. math::

     \rho u A \frac{d u}{d z} + u P' \sum_k \dot{s}_k W_k =
     - \frac{d (p A)}{dz} - \tau_w P

  where :math:`\tau_w` is the wall friction coefficient (which might be computed from
  Reynolds number based correlations).

Even though this problem extends geometrically in one direction, it can be
modeled via zero-dimensional reactors. Due to the neglecting of diffusion,
downstream parts of the reactor have no influence on upstream parts. Therefore,
PFRs can be modeled by marching from the beginning to the end of the reactor.

Cantera does not (yet) provide dedicated class to solve the PFR equations (The
``FlowReactor`` class is currently under development). However, there are two
ways to simulate a PFR with the reactor elements previously presented. Both
rely on the assumption that pressure is approximately constant throughout the
Plug-Flow Reactor and that there is no friction. The momentum conservation
equation is thus neglected.

PFR Modeling by Considering a Lagrangian Reactor
************************************************

A Plug-Flow Reactor can also be described from a Lagrangian point of view. An
unsteady fluid particle is considered which travels along the axial streamline
through the PFR. Since there is no information traveling upstream, the state
change of the fluid particle can be computed by a forward (upwind) integration
in time. Using the continuity equation, the speed of the particle can be
derived. By integrating the velocity in time, the temporal information can be
translated into the spatial resolution of the PFR.

An example for this procedure can be found in the `PFR example </examples/python/reactors/pfr.py.html>`__.

PFR Modeling as a Series of CSTRs
*********************************

The Plug-Flow Reactor is spatially discretized into a large number of axially
distributed volumes. These volumes are modeled to be steady-state CSTRs.

The only reason to use this approach as opposed to the Lagrangian one is if you
need to include surface reactions, because the system of equations ends up
being a DAE system instead of an ODE system.

In Cantera, it is sufficient to consider a single reactor and march it forward
in time, because there is no information traveling upstream. The mass flow rate
:math:`\dot{m}` through the PFR enters the reactor from an upstream reservoir. For
the first reactor, the reservoir conditions are the inflow boundary conditions
of the PFR. By performing a time integration as described in `Continuously
Stirred Tank Reactor`_ until the state of the reactor is converged, the
steady-state CSTR solution is computed. The state of the CSTR is the inlet
boundary condition for the next CSTR downstream.

An example for this procedure can be found in the `PFR example
</examples/python/reactors/pfr.py.html>`__ and the `surface PFR example
</examples/python/reactors/surf_pfr.py.html>`__.

Advanced Concepts
=================

In some cases, Cantera's solver is insufficient to describe a certain
configuration. In this situation, Cantera can still be used to provide chemical
and thermodynamic computations, but external ODE solvers can be applied. See
an example of a `custom ODE solver </examples/python/reactors/custom.py.html>`__.

.. rubric:: References

.. [Kee2017] R. J. Kee, M. E. Coltrin, P. Glarborg, and H. Zhu. *Chemically Reacting Flow:
   Theory and Practice*. 2nd Ed. John Wiley and Sons, 2017.

.. rubric:: Footnotes

.. [1] Prior to Cantera 2.6, the sense of the net heat flow was reversed, with positive
   :math:`\dot{Q}` representing heat removal from the system. However, the sense of heat
   flow through a wall between two reactors was the same, with a positive value
   representing heat flow from the left reactor to the right reactor.
