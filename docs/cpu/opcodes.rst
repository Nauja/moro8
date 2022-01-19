.. -*- coding: utf-8 -*-
.. _cpuopcodes:

=======
Opcodes
=======

moro8 is based on the 6502 so you can totally rely on its own documentation, especially the one found at 
`6502.org <http://www.6502.org/tutorials/6502opcodes.html>`_ or 
`masswerk.at <https://www.masswerk.at/6502/6502_instruction_set.html>`_, that I used myself to write this library
and that I will shamelessly copy here only for the sake of having a complete and standalone documentation.

For each opcode you will find the following table:

.. container:: moro8-opcode

    .. container:: moro8-header
        
        .. container:: moro8-pre

                ACTION
                A + M + C -> A, C

        .. container:: moro8-pre

                FLAGS
                N Z C I D V
                + + + - - +

    .. container:: moro8-synopsis moro8-pre

        MODE          SYNTAX        HEX LEN TIM
        Immediate     ADC #$44      $69  2   2
        Zero Page     ADC $44       $65  2   3
        Zero Page,X   ADC $44,X     $75  2   4
        Absolute      ADC $4400     $6D  3   4
        Absolute,X    ADC $4400,X   $7D  3   4+
        Absolute,Y    ADC $4400,Y   $79  3   4+
        Indirect,X    ADC ($44,X)   $61  2   6
        Indirect,Y    ADC ($44),Y   $71  2   5+

Where:

* **ACTION**: is a quick representation of the operation performed by the CPU.
* **FLAGS**: indicates which flag is affected **+** or not **-** or set to **0**.
* **MODE**: indicates the addressing mode used.
* **SYNTAX**: is how you write the instruction in assembler.
* **HEX**: is the hexadecimal code of the opcode depending on addressing mode.
* **LEN**: is the number of bytes for the instruction.
* **TIM**: is the number of cycles for completing the instruction.


Now, for a full list of moro8 opcodes:

.. module:: moro8

.. toctree::
   :maxdepth: 1
   :glob:

   opcodes/*
