.. -*- coding: utf-8 -*-
.. _cpy:

CPY
---

.. module:: moro8

.. contents::
   :local:

Compare Memory and Index Y.

.. container:: moro8-opcode

    .. container:: moro8-header
        
        .. container:: moro8-pre

                ACTION
                Y - M

        .. container:: moro8-pre

                FLAGS
                N Z C I D V
                + + + - - -

    .. container:: moro8-synopsis moro8-pre

                MODE          SYNTAX        HEX LEN TIM
                Immediate     CPY #$44      $C0  2   2
                Zero Page     CPY $44       $C4  2   3
                Absolute      CPY $4400     $CC  3   4
