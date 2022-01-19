.. -*- coding: utf-8 -*-
.. _bcc:

BCC
---

.. module:: moro8

.. contents::
   :local:
      
Branch on Carry Clear.

.. container:: moro8-opcode

    .. container:: moro8-header
        
        .. container:: moro8-pre

                ACTION
                branch on C = 0

        .. container:: moro8-pre

                FLAGS
                N Z C I D V
                - - - - - -

    .. container:: moro8-synopsis moro8-pre

                MODE          SYNTAX        HEX LEN TIM
                Relative      BCC $44       $90  2   2+

