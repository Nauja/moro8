.. -*- coding: utf-8 -*-
.. _txa:

TXA
---

.. module:: moro8

.. contents::
   :local:
      
Transfer Index X to Accumulator.

.. container:: moro8-opcode

    .. container:: moro8-header
        
        .. container:: moro8-pre

                ACTION
                X -> A

        .. container:: moro8-pre

                FLAGS
                N Z C I D V
                + + - - - -

    .. container:: moro8-synopsis moro8-pre
                
                MODE          SYNTAX        HEX LEN TIM
                Implied       TXA           $8A  1   2
