.. -*- coding: utf-8 -*-
.. _ldx:

LDX
---

.. contents::
   :local:

Load Index X with Memory.

.. container:: moro8-opcode

    .. container:: moro8-header
        
        .. container:: moro8-pre

                ACTION
                M -> X

        .. container:: moro8-pre

                FLAGS
                N Z C I D V
                + + - - - -

    .. container:: moro8-synopsis moro8-pre

                MODE          SYNTAX            HEX LEN TIM
                Immediate     LDX #$44          $A2  2   2
                Zero Page     LDX $44           $A6  2   3
                Zero Page,Y   LDX $44,Y         $B6  2   4
                Absolute      LDX $4400         $AE  3   4
                Absolute,Y    LDX $4400,Y       $BE  3   4+
                Stack         LDX stk,$44       $02  2   -
                Stack,X       LDX (stk,$44),Y   $12  2   -
