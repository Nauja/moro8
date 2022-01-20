.. -*- coding: utf-8 -*-
.. _dec:

DEC
---

.. module:: moro8

.. contents::
   :local:

Decrement Memory by One.

.. container:: moro8-opcode

    .. container:: moro8-header
        
        .. container:: moro8-pre

                ACTION
                M - 1 -> M

        .. container:: moro8-pre

                FLAGS
                N Z C I D V
                + + - - - -

    .. container:: moro8-synopsis moro8-pre
                
                MODE          SYNTAX        HEX LEN TIM
                Zero Page     DEC $44       $C6  2   5
                Zero Page,X   DEC $44,X     $D6  2   6
                Absolute      DEC $4400     $CE  3   6
                Absolute,X    DEC $4400,X   $DE  3   7
