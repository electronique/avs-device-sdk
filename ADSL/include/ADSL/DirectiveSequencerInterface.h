/*
 * DirectiveSequencerInterface.h
 *
 * Copyright 2017 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *     http://aws.amazon.com/apache2.0/
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#ifndef ALEXA_CLIENT_SDK_ADSL_INCLUDE_ADSL_DIRECTIVE_SEQUENCER_INTERFACE_H_
#define ALEXA_CLIENT_SDK_ADSL_INCLUDE_ADSL_DIRECTIVE_SEQUENCER_INTERFACE_H_

#include <memory>
#include <string>

#include "ADSL/DirectiveHandlerConfiguration.h"
#include "ADSL/DirectiveRouter.h"

namespace alexaClientSDK {
namespace adsl {

/**
 * Interface for sequencing and handling a stream of @c AVSDirective instances.
 *
 * Customers of this interface specify a mapping of @c AVSDirectives specified by (namespace, name) pairs to
 * instances of the @c AVSDirectiveHandlerInterface via calls to @c setDirectiveHandlers(). Changes to this
 * mapping can be made at any time by specifying a new mapping. Customers pass @c AVSDirectives in to this
 * interface for processing via calls to @c onDirective(). @c AVSDirectives are processed in the order that
 * they are received. @c AVSDirectives with a non-empty @c dialogRequestId value are filtered by the
 * @c DirectiveSequencer's current @c dialogRequestId value (specified by calls to @c setDialogRequestId()).
 * Only @c AVSDirectives with a @c dialogRequestId that is empty or which matches the last setting of the
 * @c dialogRequestId are handled. All others are ignored. Specifying a new @c DialogRequestId value while
 * @c AVSDirectives are already being handled will cancel the handling of @c AVSDirectives that have the
 * previous @c DialogRequestId and whose handling has not completed.
 *
 * This interface was factored out of DirectiveSequencer to facilitate mocking for unit tests.
 */
class DirectiveSequencerInterface {
public:
    /**
     * Destructor.
     */
    virtual ~DirectiveSequencerInterface() = default;

    /**
     * Add mappings from from @c NamespaceAndName values to @c HandlerAndPolicy values. If a mapping for any of
     * the specified @c NamespaceAndName values already exists the entire call is refused.
     *
     * @param configuration The mappings to add.
     * @return Whether the mappings were added.
     */
    virtual bool addDirectiveHandlers(const DirectiveHandlerConfiguration& configuration) = 0;

    /**
     * Remove the specified mappings from @c NamespaceAndName values to @c HandlerAndPolicy values. If any of
     * the specified mappings do not match an existing mapping, the entire operation is refused.
     *
     * @param configuration the mappings to remove.
     * @return Whether the mappings were removed.
     */
    virtual bool removeDirectiveHandlers(const DirectiveHandlerConfiguration& configuration) = 0;

    /**
     * Set the current @c DialogRequestId. This value can be set at any time. Setting this value causes a
     * @c DirectiveSequencer to drop unhandled @c AVSDirectives with different (and non-empty) DialogRequestId
     * values.  @c AVSDirectives with a differing @c dialogRequestId value and whose pre-handling or handling
     * is already in progress will be cancelled.
     *
     * @param dialogRequestId The new value for the current @c DialogRequestId.
     */
    virtual void setDialogRequestId(const std::string& dialogRequestId) = 0;

    /**
     * Sequence the handling of an @c AVSDirective.  The actual handling is done by whichever @c DirectiveHandler
     * is associated with the @c AVSDirective's (namespace, name) pair.
     *
     * @param directive The @c AVSDirective to handle.
     * @return Whether or not the directive was accepted.
     */
    virtual bool onDirective(std::shared_ptr<avsCommon::AVSDirective> directive) = 0;

    /**
     * Shut down the DirectiveSequencer.  This method blocks until all processing of directives has stopped.
     */
    virtual void shutdown() = 0;
};

} // namespace adsl
} // namespace alexaClientSDK

#endif // ALEXA_CLIENT_SDK_ADSL_INCLUDE_ADSL_DIRECTIVE_SEQUENCER_INTERFACE_H_