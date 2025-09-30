Zephyr devicetree bindings
==========================

Sometimes, your application requires cutomizing the definitions of the
board/SoC devicetree nodes. Therefore, you might find yourself
needing to add some additional properties to the node definitions.

In the context of Zephyr, a **devicetree binding** is a YAML file, which is
used to tell the build system which properties a devicetree node is allowed
to have. Apart from this, a devicetree binding also serves as documentation,
telling the developers which properties they're allowed to use and providing
brief documentation on said properties.

Most importantly, a devicetree binding is specific to a certain programming
model. Consequently, if you want to identify the devicetree binding for a
certain devicetree node, you'll have to look at the value of the ``compatible``
property.

Let's assume we're interested in the ``tpm3`` node. Its definition is shown below:

.. code-block:: devicetree

   /* node definition taken from nxp_mimx93_a55.dtsi */

   tpm3: tpm@424e0000 {
       compatible = "nxp,tpm-timer";
       reg = <0x424e0000 DT_SIZE_K(64)>;
       interrupts = <GIC_SPI 75 IRQ_TYPE_LEVEL IRQ_DEFAULT_PRIORITY>;
       interrupt-names = "irq_0";
       interrupt-parent = <&gic>;
       clocks = <&ccm IMX_CCM_TPM3_CLK 0 0>;
       prescaler = <1>;
       status = "disabled";
   };

Looking at the value of the ``compatible`` property, we can see that the
configured programming model is ``nxp,tpm-timer``.

In Zephyr, the root directory of the devicetree bindings can be found under:

.. code-block:: text

   ~/nxpcup_root/zephyr/dts/bindings/

with all the devicetree binding files being organized in subdirectories,
one for each subsystem.

Going back to our example, assuming we don't known the subsystem for our
binding, we have two options:

1. Use ``git grep``:

.. tabs::

   .. group-tab:: Linux

      .. code-block:: bash

         # run this inside ~/nxpcup_root/zephyr
         git grep nxp,tpm-timer -- dts/bindings

   .. group-tab:: Windows

      .. code-block:: powershell

         # run this inside ~\Desktop\nxpcup_root\zephyr
         git grep nxp,tpm-timer -- dts\bindings

In which case, the output should be similar to:

.. code-block:: text

   dts/bindings/counter/nxp,tpm-timer.yaml:compatible: "nxp,tpm-timer"

What we're interested here is finding the YAML file that contains a
``compatible: <compatible_value>`` line, where ``compatible_value`` is
the value of our compatible (e.g. ``nxp,tpm-timer``).

To confirm that the file returned by the ``git grep`` command is the
right one, we can look at its name. Usually, the name of the binding
matches the name of the programming model (e.g. ``nxp,tpm-timer``).

2. Use ``find``:

The name of the devicetree binding will usually match the name of the
programming model. We can use this to search for the binding directly
using ``find``:

.. tabs::

   .. group-tab:: Linux

      .. code-block:: bash

         # run this inside ~/nxpcup_root/zephyr
         find dts/bindings -name nxp,tpm-timer.yaml

   .. group-tab:: Windows

      .. code-block:: powershell

         # run this inside ~\Desktop\nxpcup_root\zephyr
         Get-ChildItem -Path .\dts\bindings\ -Recurse -Name "nxp,tpm-timer.yaml"

If everything went well, you should see the path to the binding file:

.. code-block:: text

   dts/bindings/counter/nxp,tpm-timer.yaml

Inspecting the content of a binding file
----------------------------------------

Now that we've indentified the devicetree binding for our programming
model, it's time to look at its content:

.. code-block:: yaml

   # taken from nxp,tpm-timer.yaml - some keys were intentionally omitted

   description: NXP Timer/PWM Module (TPM) used as timer

   compatible: "nxp,tpm-timer"

   include: base.yaml

   properties:
     reg:
       required: true

     interrupts:
       required: true

     clocks:
       required: true

     prescaler:
       type: int
       required: true
       enum:
         - 1
         - 2
         - 4
         - 8
         - 16
         - 32
         - 64
         - 128
       description: Input clock prescaler

The ``description`` key is used to provide additional information on the
underlying hardware. In this particular case, we're working with a timer/PWM
module from NXP.

The ``compatible`` key is used by the build system to identify which devicetree
nodes this binding applies to. In our example, this binding would apply to
all devicetree node with the ``compatible`` value containing the ``nxp,tpm-timer``
string.

The ``include`` key is used to import property definitions from other
binding files. Here, we're importing the content of ``base.yaml``,
which contains the definitions for standard properties such as
``clock``, ``interrupts``, etc..

The ``properties`` key is used to define properties or modify imported
ones. This is the key we'll be most interested in as it contains the
list of properties we're allowed to specify in our devicetree nodes.
If we're working with imported properties (i.e. we have an ``include``
key in our binding), then we also need to consider the properties
defined in the included binding file. For example, the ``compatible``
property is defined in ``base.yaml`` and imported in ``nxp,tpm-timer.yaml``,
which is why we're allowed to use the ``compatible`` property in our
``tpm3`` node.

In this particular case, we have 3 standard propeties (defined in ``base.yaml``):
``reg``, ``interrupts``, ``clocks``, which are modified by our ``nxp,tpm-timer.yaml``
binding file. By default, these properties are optional (i.e. you're not forced to
add these properties to your devicetree node), but, by adding the ``required: true``
line, we're making them mandatory. Therefore, each devicetree node using the
``nxp,tpm-timer`` programming model must have these properties. Otherwise,
building your application will fail.

Apart from the standard properties, we also have the definition of a non-standard
property: ``prescaler``. As you can probably deduce from its definition, this
property takes an ``int`` from the ``{1, 2, 4, 8, 16, 32, 64, 128}`` set as
its value and is mandatory.

Further reading
---------------

For more information, check out the following documentation pages from Zephyr:

1. https://docs.zephyrproject.org/latest/build/dts/bindings-syntax.html
