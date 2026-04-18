/**
 *
 *  @file template/template.hpp
 *  @author Gaspard Kirira
 *
 *  @brief Internal aggregation header for the Vix template module.
 *
 *  This file includes the core template engine components provided by Vix,
 *  including parsing, compilation, rendering, execution, caching, loading,
 *  and value handling.
 *
 *  For most use cases, prefer:
 *    #include <vix/template.hpp>
 *
 *  Copyright 2025, Gaspard Kirira.
 *  All rights reserved.
 *  https://github.com/vixcpp/vix
 *
 *  Use of this source code is governed by a MIT license
 *  that can be found in the License file.
 *
 *  Vix.cpp
 */
#ifndef VIX_TEMPLATE_TEMPLATE_HPP
#define VIX_TEMPLATE_TEMPLATE_HPP

#include <vix/template/AST.hpp>
#include <vix/template/Builtins.hpp>
#include <vix/template/Cache.hpp>
#include <vix/template/Compiler.hpp>
#include <vix/template/Context.hpp>
#include <vix/template/Engine.hpp>
#include <vix/template/Environment.hpp>
#include <vix/template/Error.hpp>
#include <vix/template/Escape.hpp>
#include <vix/template/ExecutionPlan.hpp>
#include <vix/template/FileSystemLoader.hpp>
#include <vix/template/Instruction.hpp>
#include <vix/template/Lexer.hpp>
#include <vix/template/Loader.hpp>
#include <vix/template/Optimizer.hpp>
#include <vix/template/Parser.hpp>
#include <vix/template/Renderer.hpp>
#include <vix/template/RenderResult.hpp>
#include <vix/template/StringLoader.hpp>
#include <vix/template/Template.hpp>
#include <vix/template/Token.hpp>
#include <vix/template/Value.hpp>
#include <vix/template/Version.hpp>

#endif // VIX_TEMPLATE_TEMPLATE_HPP
