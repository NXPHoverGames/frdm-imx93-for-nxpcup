Zephyr FRDM-IMX93 devicetree files
==================================

Throughout this guide we'll assume that you have the work directory
placed under ``~/work/repos/nxpcup_root`` (Linux) or
``~\Desktop\nxpcup_root`` (Windows) and that you have run the setup
script according to :ref:`preparing-environment-linux` or
:ref:`preparing-the-environment-windows`.

Board and SoC devicetrees
-------------------------

SoCs by themselves are not really useful, which is why you'll usually see
them integrated into boards. Since the same SoC may be integrated into
multiple, different boards, it makes sense from the devicetree's perspective
to have the hardware devices that make up the SoC described in a DTSI, which
can then be included and configured in the board DTS. This way, for a set of
boards integrating the same SoC, you'll end up with a centralized location
for all of the common hardware device nodes.

In the context of Zephyr, you'll notice that each SoC has an associated DTSI,
while each board has a DTS. Usually, the board DTS will include the SoC DTSI.

Furthermore, each SoC may have multiple CPU clusters, each with different core
architectures. For example, the i.MX93 SoC has two Cortex-A55 cores, which use
the ``arm64`` architecture (i.e. 64-bit ARM) and one Cortex-M33 core, which uses
the ``arm`` architecture (i.e. 32-bit ARM). Because of this, in the context of
Zephyr, each SoC will have a per-cluster SoC DTSI, describing that CPU cluster's
view of the hardware.

For NXP SoCs, the DTSIs are found under: 

.. tabs::

      .. group-tab:: Linux

         .. code-block:: text

            ~/work/repos/nxpcup_root/zephyr/dts/<arch>/nxp/

      .. group-tab:: Windows

         .. code-block:: text

            ~\Desktop\nxpcup_root\zephyr\dts\<arch>\nxp\

, where ``arch`` is the name of the CPU cluster's architecture (e.g. ``arm``,
``arm64`` etc..). Since this project uses the Cortex-A55 cluster, the name of
our targeted architecture will be ``arm64``.

Based on this, we can find the i.MX93 Cortex-A55 SoC DTSI under:

.. tabs::

      .. group-tab:: Linux

         .. code-block:: text

	    ~/work/repos/nxpcup_root/zephyr/dts/arm64/nxp/nxp_mimx93_a55.dts

      .. group-tab:: Windows

         .. code-block:: text

            ~\Desktop\nxpcup_root\zephyr\dts\arm64\nxp\nxp_mimx93_a55.dts

On the other hand, for NXP boards, the DTS are found under:

.. tabs::

      .. group-tab:: Linux

         .. code-block:: text

	    ~/work/repos/nxpcup_root/zephyr/boards/nxp/<board_name>

      .. group-tab:: Windows

         .. code-block:: text

            ~\Desktop\nxpcup_root\zephyr\boards\nxp\<board_name>

, where ``board_name`` is the name of the targeted board, which, for FRDM-IMX93
would be ``frdm_imx93``. Therefore, we can find the FRDM-IMX93 board DTS under:

.. tabs::

      .. group-tab:: Linux

         .. code-block:: text

	    ~/work/repos/nxpcup_root/zephyr/boards/nxp/frdm_imx93/frdm_imx93_mimx9352_a55.dts

      .. group-tab:: Windows

         .. code-block:: text

            ~\Desktop\nxpcup_root\zephyr\boards\nxp\frdm_imx93\frdm_imx93_mimx9352_a55.dts

Inspecting the i.MX93 SoC DTSI
------------------------------

Now that we have identified its location, we can have a look at the i.MX93
SoC DTSI. Upon dumping the content of ``nxp_mimx93_a55.dtsi`` DTSI, the
first thing we notice is the included header files and DTSIs:

.. code-block:: devicetree

   /* taken from nxp_mimx93_a55.dtsi */

   #include <mem.h>
   #include <freq.h>
   #include <arm64/armv8-a.dtsi>
   #include <zephyr/dt-bindings/clock/imx_ccm_rev2.h>
   #include <zephyr/dt-bindings/interrupt-controller/arm-gic.h>
   #include <zephyr/dt-bindings/gpio/gpio.h>
   #include <zephyr/dt-bindings/i2c/i2c.h>

which are used to pull in the required macro (e.g. ``GIC_PPI``, ``DT_SIZE_K`` etc..)
and devicetree node (e.g. ``armv8-a.dtsi``) definitions. This is then followed by
the definitions of the devicetree nodes:

1. **The root node**:

.. code-block:: devicetree

   /* taken from nxp_mimx93_a55.dtsi */
   
   / {
         #address-cells = <1>;
         #size-cells = <1>;

         /* other nodes were intentionally omitted */
   };

Based on the value of the ``#address-cells`` and ``#size-cells`` properties,
we can conclude that the children of the root node (not applicable to all
descendants, however) will have 32-bit addresses and sizes.

2. **The CPUs node**:

.. code-block:: devicetree

   /* taken from nxp_mimx93_a55.dtsi */

   / {
         /* other nodes/properties were intentionally omitted */

         cpus {
             #address-cells = <1>;
             #size-cells = <0>;

             cpu@0 {
                 device_type = "cpu";
                 compatible = "arm,cortex-a55";
                 reg = <0>;
             };

             cpu@100 {
                 device_type = "cpu";
                 compatible = "arm,cortex-a55";
                 reg = <0x100>;
             };
         };
   };

The Cortex-A55 cluster is made up of two cores, which is why the ``cpus`` node
has exactly two children: ``cpu@0`` and ``cpu@100``. Furthermore, to distinguish
between these two nodes, the ``@unit-address`` bit had to be added to the node
names. Since there's no need for a size item in the ``reg`` property (as the address
is used as identification), the value of the ``#size-cells`` is set to 0.

3. **The peripheral nodes**:

.. code-block:: devicetree

   /* taken from nxp_mimx93_a55.dtsi */

   / {
       /* other nodes/properties were intentionally omitted */

       gpio2: gpio@43810000 {
           compatible = "nxp,imx-rgpio";
           reg = <0x43810000 DT_SIZE_K(64)>;
           interrupt-parent = <&gic>;
           interrupts = <GIC_SPI 57 IRQ_TYPE_LEVEL IRQ_DEFAULT_PRIORITY>,
                        <GIC_SPI 58 IRQ_TYPE_LEVEL IRQ_DEFAULT_PRIORITY>;
           gpio-controller;
           #gpio-cells = <2>;
           status = "disabled";
       };
   };

.. note::

   For the sake of brevity, the snippet above includes only one peripheral node.

Based on the properties found inside the ``gpio2`` node, we can extract
the following information:

* ``compatible = "nxp,imx-rgpio"``: the programming model is ``"nxp,imx-rgpio"``.
* ``reg = <0x43810000 DT_SIZE_K(64)>``: the address space spans from ``0x43810000``
  to ``0x43820000`` and its size is 64KB.
* ``interrupt-parent = <&gic>``: the peripheral signals its interrupts to the CPU
  via the GIC (which is the interrupt controller for the ARMv8-A architecture).
* ``interrupts = <...>, <...>``: the interrupt lines used by this peripheral
  are 57 and 58.
* ``gpio-controller``: this peripheral is a GPIO controller.
* ``status = "disabled"``: this peripheral is disabled. Usually, the board DTS
  will enable the peripheral nodes.


Looking for additional information
----------------------------------

Sometimes, the information provided by the devicetree on a certain peripheral
may not be enough. For instance, we might be interested in how said peripheral
works. In such cases, we can use the information provided by the DTS to look
up the underlying hardware device in the SoC's reference manual.

As an example, let's assume we're interested in learning more about the
underlying hardware device for the ``gpio2`` devicetree node. The first
step would be to download the SoC's reference manual. For i.MX93, you can
get the reference manual from `here <https://www.nxp.com/webapp/Download?colCode=IMX93RM>`__.

The next step is to try and figure out the peripheral's name. Sometimes,
this information is encoded in the devicetree node's label name. For the
``gpio2`` node, the name of the peripheral can be extracted by removing
all of the trailing digits from its label name, thus yielding: GPIO.

.. note::

   The devicetree node label names are sometimes the same as the
   names of the underlying hardware devices plus some digits used
   to distinguish between the various instances of the same peripheral.

Once we have the name of the peripheral (or its acronym in this
particular case), we can try to look for a chapter or section that
contains this name (or acronym) inside the reference manual. In our
particular case, that would be **Chapter 28**, **General Purpose Input-Output (GPIO)**.

If, for whatever reason, the devicetree node label name is not the
same as the peripheral's name, we can try to find it by using its
base address. For the ``gpio2`` node, this would be ``0x43810000``.

After obtaining the base address, we can inspect the system's memory
map and look for a device that has the same base address. For i.MX93,
the system memory map is described in **Chapter 2**, **Memory Maps**.
You'll have to look through the tables defined in said chapter and try
to find the appropriate entry. Once you do that, you may find the name
of the device in the **Description**/**NIC port** column. For now, you
can ignore the addresses in the reference manual that have the **(S)**
bit since we won't be working with these.

Inspecting the FRDM-IMX93 board DTS
-----------------------------------

After inspecting the SoC DTSI, we should also look at the board DTS to see
which devices have their status set to ``okay``, since these will be the
devices that Zephyr will be dealing with.

Upon dumping the content of ``frdm_imx93_mimx9352_a55.dts``, the first
thing we notice is the included header files and DTSIs:

.. code-block:: devicetree

   /* taken from frdm_imx93_mimx9352_a55.dts */

   /* this bit is mandatory for all .dts files */
   /dts-v1/;

   #include <nxp/nxp_mimx93_a55.dtsi>
   #include "frdm_imx93-pinctrl.dtsi"
   #include <zephyr/dt-bindings/input/input-event-codes.h>

Most notably, we can finally confirm that the board DTS does indeed use
the SoC DTSI by looking at the ``#include <nxp/nxp_mimx93_a55.dtsi>`` bit.

The header inclusion bit is then followed by the definitions of the
devicetree nodes:

1. **The CPUs node**:

.. code-block:: devicetree

   /* taken from frdm_imx93_mimx9352_a55.dts */

   / {
       /* other nodes/properties were intentionally omitted */

       cpus {
           cpu@0 {
               status = "disabled";
           };
       };
   };

We can see here that one of the CPU cores gets disabled, meaning Zephyr
will only run on of the Cortex-A55 cores (since the other one still has
its status set to ``okay``).

2. **The memory node**:

.. code-block:: devicetree

   /* taken from frdm_imx93_mimx9352_a55.dts */

   / {
       /* other nodes/properties were intentionally omitted */

       dram: memory@d0000000 {
            reg = <0xd0000000 DT_SIZE_M(1)>;
       };
   };

Based on this node's definition, we can conclude that Zephyr's RAM will
start from ``0xd0000000`` and end at ``0xd0100000``, thus having a size
of 1MB.

3. **The perioheral nodes**:

.. code-block:: devicetree

   /* taken from frdm_imx93_mimx9352_a55.dts */

   /* other nodes/properties were intentionally omitted */

   &gpio2 {
       status = "okay";
   };

.. note::

   For the sake of brevity, the snippet above only includes one peripheral node.

What's interesting to note here is that, as expected, the peripheral (which had
its status set to ``disabled`` in the SoC DTSI) now has its status set to ``okay``.

Devicetree overlays
-------------------

Having one devicetree source file that satisfies the needs of all applications
is no easy feat. This is because applications can have different requirements
with respect to what devices need to be enabled (i.e. have their status set to
``okay``) and how the nodes need to be configured. While having a per-application
board DTS is possible, Zephyr also allows us to use devicetree overlays. This
way, the board DTS remains unchanged, while each application will have a devicetree
overlay with all of the required changes.

The syntax of a devicetree overlay is pretty much the same as that of a DTS or a
DTSI. The only difference here is that you don't need to include the board DTS
or the SoC DTSI (via the ``#include`` preprocessor directive) inside your devicetree
overlay since it is assumed that the nodes you're using are already defined in
the DTS you're applying the overlay to.

For instance, let's assume we want to create a devicetree overlay, which re-configures
the ``tpm3`` node defined in the SoC DTSI. The first step is to look at the SoC DTSI
and board DTS and figure out its current configuration. Since the node is not modified
inside the board DTS, its configuration will end up being:

.. code-block:: devicetree

   /* assembled from nxp_mimx93_a55.dtsi and frdm_imx93_mimx9352_a55.dts */

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

Assuming we want to change the programming model and set its status to ``okay``,
we'd create a devicetree overlay in our application directory (as described in
:ref:`writing-your-application`) with the following content:

.. code-block:: devicetree

   /* content of our example frdm_imx93.overlay file */

   /* adapted from samples/hbridge/frdm_imx93.overlay */

   /* we're not including the board DTS or the SoC DTSI!!!! */

   &tpm3 {
       compatible = "nxp,kinetis-tpm";
       #pwm-cells = <3>;
       status = "okay";
   };
