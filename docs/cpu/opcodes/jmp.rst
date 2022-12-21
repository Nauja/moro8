.. -*- coding: utf-8 -*-
.. _jmp:

JMP
---

.. contents::
   :local:

Jump to New Location.

.. container:: moro8-opcode

    .. container:: moro8-header
        
        .. container:: moro8-pre

                ACTION
                (PC+1) -> PCL
                (PC+2) -> PCH

        .. container:: moro8-pre

                FLAGS
                N Z C I D V
                - - - - - -

    .. container:: moro8-synopsis moro8-pre

                MODE          SYNTAX        HEX LEN TIM
                Absolute      JMP $5597     $4C  3   3
                Absolute,X    JMP $5597,X   $7C  3   6
                Indirect      JMP ($5597)   $6C  3   5

.. note::

    Not a 6502 addressing mode: (abs,X).
