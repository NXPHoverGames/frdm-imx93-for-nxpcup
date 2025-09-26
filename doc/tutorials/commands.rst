.. _command-cheatsheet:

Command cheatsheet
==================

Using ``west``
--------------

1. **Update the repositories:**

.. code-block:: bash

   west update

2. **Build an application:**

.. code-block:: bash

   west build -p -b frdm_imx93//a55 <path_to_your_application>

3. **Build an application and apply a devicetree overlay:**

.. code-block:: bash

   west build -p -b frdm_imx93//a55 <path_to_your_sample> -D DTC_OVERLAY_FILE=<overlay>

4. **Build an application and save preprocessed files:**

.. code-block:: bash

      west build -p -b frdm_imx93//a55 <path_to_your_sample> -D CONFIG_COMPILER_SAVE_TEMPS=y

Using ``docker``
----------------

1. **Build a Docker image and run the container:**

.. code-block:: bash

   docker compose -f ./docker/compose.yaml up --detach

2. **Shut down a container:**

.. code-block:: bash

   docker compose -f ./docker/compose.yaml down

3. **Open a shell inside a container:**

.. code-block:: bash

   docker compose -f ./docker/compose.yaml exec zephyr bash

4. **Run a command inside a container:**

.. code-block:: bash

   docker dompose -f ./docker/compose.yaml exec zephyr bash -c "<command_to_run>"
