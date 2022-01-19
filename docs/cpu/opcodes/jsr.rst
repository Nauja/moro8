.. -*- coding: utf-8 -*-
.. _jsr:

JSR
---

.. module:: moro8

.. contents::
   :local:

Jump to New Location Saving Return Address.

.. container:: moro8-opcode

    .. container:: moro8-header
        
        .. container:: moro8-pre

                ACTION
                push (PC+2),
                (PC+1) -> PCL
                (PC+2) -> PCH

        .. container:: moro8-pre

                FLAGS
                N Z C I D V
                - - - - - -

    .. container:: moro8-synopsis moro8-pre

                MODE          SYNTAX        HEX LEN TIM
                Absolute      JSR $5597     $20  3   6

