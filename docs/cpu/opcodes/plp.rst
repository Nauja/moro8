.. -*- coding: utf-8 -*-
.. _plp:

PLP
---

.. module:: moro8

.. contents::
   :local:
      
Pull Processor Status from Stack.

.. container:: moro8-opcode

    .. container:: moro8-header
        
        .. container:: moro8-pre

                ACTION
                pull SR

        .. container:: moro8-pre

                FLAGS
                N Z C I D V
                 from stack

    .. container:: moro8-synopsis moro8-pre
                
                MODE          SYNTAX        HEX LEN TIM
                Implied       PLP           $28  1   4

The status register will be pulled with the break
flag and bit 5 ignored.
