.. -*- coding: utf-8 -*-
.. _beq:

BEQ
---

.. module:: moro8

.. contents::
   :local:

Branch on Result Zero.

.. container:: moro8-opcode

    .. container:: moro8-header
        
        .. container:: moro8-pre

                ACTION
                branch on Z = 1

        .. container:: moro8-pre

                FLAGS
                N Z C I D V
                - - - - - -

    .. container:: moro8-synopsis moro8-pre

                MODE          SYNTAX        HEX LEN TIM
                Relative      BEQ $44       $F0  2   2+

