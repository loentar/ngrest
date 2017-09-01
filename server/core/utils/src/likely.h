/*
 *  Copyright 2016 Utkin Dmitry <loentar@gmail.com>
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/*
 *  This file is part of ngrest: http://github.com/loentar/ngrest
 */

#ifndef NGREST_UTILS_LIKELY_H
#define NGREST_UTILS_LIKELY_H

#ifndef NGREST_LIKELY
#ifdef __GNUC__
#define NGREST_LIKELY(EXPRESSION) __builtin_expect(!!(EXPRESSION), true)
#define NGREST_UNLIKELY(EXPRESSION) __builtin_expect(!!(EXPRESSION), false)
#else
#define NGREST_LIKELY(EXPRESSION) (EXPRESSION)
#define NGREST_UNLIKELY(EXPRESSION) (EXPRESSION)
#endif
#endif

#endif // NGREST_UTILS_LIKELY_H
