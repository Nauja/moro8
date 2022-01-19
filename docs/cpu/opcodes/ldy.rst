.. -*- coding: utf-8 -*-
.. _ldy:

LDY
---

.. module:: moro8

.. contents::
   :local:

Load Index Y with Memory.

.. container:: moro8-opcode

    .. container:: moro8-header
        
        .. container:: moro8-pre

                ACTION
                M -> Y

        .. container:: moro8-pre

                FLAGS
                N Z C I D V
                + + - - - -

    .. container:: moro8-synopsis moro8-pre

                MODE          SYNTAX        HEX LEN TIM
                Immediate     LDY #$44      $A0  2   2
                Zero Page     LDY $44       $A4  2   3
                Zero Page,X   LDY $44,X     $B4  2   4
                Absolute      LDY $4400     $AC  3   4
                Absolute,X    LDY $4400,X   $BC  3   4+
