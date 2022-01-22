.. -*- coding: utf-8 -*-
.. _cpx:

CPX
---

.. module:: moro8

.. contents::
   :local:

Compare Memory and Index X.

.. container:: moro8-opcode

    .. container:: moro8-header
        
        .. container:: moro8-pre

                ACTION
                X - M

        .. container:: moro8-pre

                FLAGS
                N Z C I D V
                + + + - - -

    .. container:: moro8-synopsis moro8-pre

                MODE          SYNTAX        HEX LEN TIM
                Immediate     CPX #$44      $E0  2   2
                Zero Page     CPX $44       $E4  2   3
                Absolute      CPX $4400     $EC  3   4
