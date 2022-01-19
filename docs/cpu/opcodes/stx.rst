.. -*- coding: utf-8 -*-
.. _stx:

STX
---

.. module:: moro8

.. contents::
   :local:
      
Store Index X in Memory.

.. container:: moro8-opcode

    .. container:: moro8-header
        
        .. container:: moro8-pre

                ACTION
                X -> M

        .. container:: moro8-pre

                FLAGS
                N Z C I D V
                - - - - - -

    .. container:: moro8-synopsis moro8-pre

                MODE          SYNTAX        HEX LEN TIM
                Zero Page     STX $44       $86  2   3
                Zero Page,Y   STX $44,Y     $96  2   4
                Absolute      STX $4400     $8E  3   4
