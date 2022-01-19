.. -*- coding: utf-8 -*-
.. _jmp:

JMP
---

.. module:: moro8

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
                Indirect      JMP ($5597)   $6C  3   5

