.. -*- coding: utf-8 -*-
.. _sty:

STY
---

.. module:: moro8

.. contents::
   :local:
      
Store Index Y in Memory.

.. container:: moro8-opcode

    .. container:: moro8-header
        
        .. container:: moro8-pre

                ACTION
                Y -> M

        .. container:: moro8-pre

                FLAGS
                N Z C I D V
                - - - - - -

    .. container:: moro8-synopsis moro8-pre

                MODE          SYNTAX        HEX LEN TIM
                Zero Page     STY $44       $84  2   3
                Zero Page,X   STY $44,X     $94  2   4
                Absolute      STY $4400     $8C  3   4
