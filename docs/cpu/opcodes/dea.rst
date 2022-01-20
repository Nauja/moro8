.. -*- coding: utf-8 -*-
.. _dea:

DEA
---

.. module:: moro8

.. contents::
   :local:

Decrement Accumulator by One.

.. container:: moro8-opcode

    .. container:: moro8-header
        
        .. container:: moro8-pre

                ACTION
                A - 1 -> A

        .. container:: moro8-pre

                FLAGS
                N Z C I D V
                + + - - - -

    .. container:: moro8-synopsis moro8-pre
                
                MODE          SYNTAX        HEX LEN TIM
                Implied       DEA           $3A  1   2

.. note::

    Not a 6502 opcode.
